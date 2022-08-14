#include "builder.hpp"
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <iostream>
#include <regex>
#include "render_file.hpp"
#include "../file_renderer.hpp"
#include "utils/string.hpp"

using namespace std;

// html:
// crails templates -r html -i app/views -t Crails::HtmlTemplate -z crails/html_template.hpp
// json:
// crails templates -r json -i app/views -t Crails::JsonTemplate -z crails/json_template.hpp

void TemplateBuilder::options_description(boost::program_options::options_description& desc) const
{
  desc.add_options()
    ("template-type,t",   boost::program_options::value<string>(), "name of the template class (defaults to Crails::Template)")
    ("template-header,z", boost::program_options::value<string>(), "path to the header defining the template class")
    ("render-mode,m",     boost::program_options::value<string>(), "use raw or markup mode")
    ("pattern,p",         boost::program_options::value<string>(), "pattern matching input files")
    ("input,i",           boost::program_options::value<string>(), "input folders, separated by commas")
    ("renderer,r",        boost::program_options::value<string>(), "renderer name (ex: html)");
}

int TemplateBuilder::run()
{
  configuration.initialize();
  if (validate_options() && generate_templates())
    return generate_renderer_ctor() ? 0 : -1;
  return -1;
}

bool TemplateBuilder::validate_options()
{
  if (!options.count("renderer"))
  {
    cerr << "No renderer specified." << endl;
    return false;
  }
  if (!options.count("input"))
  {
    cerr << "No input directory specified." << endl;
    return false;
  }
  if (options.count("pattern"))
    pattern = options["pattern"].as<string>();
  renderer = options["renderer"].as<string>();
  return true;
}

void TemplateBuilder::collect_files()
{
  boost::filesystem::path root_path(options["input"].as<string>());

  collect_files(boost::filesystem::canonical(root_path));
}

void TemplateBuilder::collect_files(boost::filesystem::path directory)
{
  boost::filesystem::path root_path(options["input"].as<string>());
  boost::filesystem::recursive_directory_iterator dir(directory);
  std::regex matcher(pattern);
  unsigned int root_path_length;

  root_path = boost::filesystem::canonical(root_path);
  root_path_length = root_path.string().length();
  for (auto& entry : dir)
  {
    std::string filepath = boost::filesystem::canonical(entry.path()).string();
    std::string filename = entry.path().filename().string();
    std::string alias, classname;
    auto match = std::sregex_iterator(filename.begin(), filename.end(), matcher);

    if (match == std::sregex_iterator())
      continue ;
    if (boost::filesystem::is_directory(entry))
    {
      collect_files(entry.path());
      continue ;
    }
    alias = filepath.substr(root_path_length + 1);
    for (unsigned int i = 0 ; i < alias.length() ; ++i)
    {
      if (alias[i] == '/' || alias[i] == '.') classname += '_';
      else classname += alias[i];
    }
    alias = alias.substr(0, alias.length() - match->length());
    classname = Crails::uppercase(classname);
    targets.emplace(filepath, Target{alias, classname, Crails::underscore(classname)});
  }
}

string TemplateBuilder::command_for_target(const pair<string, Target>& target) const
{
  string ecpp_binary = configuration.crails_bin_path() + "/ecpp";
  stringstream command;

  command << ecpp_binary << " -n " << target.second.classname << " -i " << target.first;
  if (options.count("render-mode"))
    command << " -m " << options["render-mode"].as<string>();
  if (options.count("template-type"))
    command << " -t " << options["template-type"].as<string>();
  if (options.count("template-header"))
    command << " -z " << options["template-header"].as<string>();
  return command.str();
}

bool TemplateBuilder::run_ecpp(const pair<string, Target>& target, Crails::RenderFile& output) const
{
  boost::process::ipstream pipe_stream;
  boost::process::child    ecpp(
    command_for_target(target),
    boost::process::std_out > pipe_stream
  );
  string       line;
  stringstream generated_template;

  while (pipe_stream && std::getline(pipe_stream, line))
    generated_template << line << '\n';
  ecpp.wait();
  if (ecpp.exit_code() != 0)
    return false;
  output.set_body(generated_template.str().c_str(), generated_template.str().length());
  return true;
}

void TemplateBuilder::prune_up_to_date_template(Targets::iterator it, boost::filesystem::path template_path)
{
  boost::filesystem::path directory(output_directory + '/' + renderer);
  boost::filesystem::path existing_template_path(it->first);

  if (boost::filesystem::last_write_time(existing_template_path) < boost::filesystem::last_write_time(template_path))
  {
    cout << "[TEMPLATE] " << it->second.alias << " already up to date." << endl;
    targets.erase(it);
  }
}

void TemplateBuilder::clear_dropped_templates()
{
  boost::filesystem::path directory(output_directory + '/' + renderer);
  boost::filesystem::recursive_directory_iterator dir(directory);
  list<boost::filesystem::path> marked_for_deletion;

  for (auto& entry : dir)
  {
    string filename = entry.path().filename().string();
    bool   exists = false;
    auto   it = targets.begin();

    for (; it != targets.end() ; ++it)
    { if ((it->second.function + ".cpp") == filename) break ; }
    for (auto it = targets.begin() ; it != targets.end() && !exists ; ++it)
      exists = (it->second.function + ".cpp") == filename;
    if (it == targets.end())
      marked_for_deletion.push_back(entry.path());
    else
      prune_up_to_date_template(it, entry.path());
  }
  for (const auto& path : marked_for_deletion)
  {
    cout << "[TEMPLATE] Clearing " << path.string() << endl;
    boost::filesystem::remove(path);
  }
}

bool TemplateBuilder::generate_templates()
{
  string renderer_output_directory = output_directory + '/' + renderer;

  collect_files();
  if (boost::filesystem::is_directory(renderer_output_directory))
    clear_dropped_templates();
  for (auto it = targets.begin() ; it != targets.end() ; ++it)
  {
    string             generated_template;
    auto               target = pair{it->first, it->second};
    Crails::RenderFile render_target;

    cout << "[TEMPLATE] Generating template " << target.second.alias << endl;
    boost::filesystem::create_directories(renderer_output_directory);
    if (!boost::filesystem::is_directory(renderer_output_directory))
    {
      cerr << "[TEMPLATE] Could not create directory " << renderer_output_directory << endl;
      return false;
    }
    render_target.open(renderer_output_directory + '/' + target.second.function + ".cpp");
    if (!run_ecpp(target, render_target))
    {
      cerr << "[TEMPLATE] Could not generate " << target.second.alias << endl;
      return false;
    }
  }
  return true;
}

bool TemplateBuilder::generate_renderer_ctor()
{
  FileRenderer renderer;
  map<string, string>  target_var;

  for (auto it = targets.begin() ; it != targets.end() ; ++it)
    target_var.emplace(it->second.alias, Crails::underscore(it->second.classname));
  renderer.should_overwrite = true;
  renderer.vars["renderer_name"] = this->renderer;
  renderer.vars["targets"] = &target_var;
  renderer.generate_file("renderer.cpp", output_directory + '/' + this->renderer + ".cpp");
  return true;
}
