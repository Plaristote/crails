#include "new.hpp"
#include <boost/filesystem.hpp>
#include "version.hpp"
#include "utils/string.hpp"
#include <regex>
#include <iostream>

using namespace std;

New::New() : vars(renderer.vars)
{
  vars["crails_version"] = string(LIBCRAILS_VERSION_STR);
  vars["renderers"] = &project_renderers;
  vars["parsers"]   = &project_parsers;
  vars["handlers"]  = &project_handlers;
}

void New::options_description(boost::program_options::options_description& desc) const
{
  desc.add_options()
    ("name,n",          boost::program_options::value<string>(), "project name")
    ("target,o",        boost::program_options::value<string>(), "folder in which the project will be generated (equal to `name` by default)")
    ("toolchain,t",     boost::program_options::value<string>(), "options: cmake or build2 (defaults to cmake)")
    ("configuration,c", boost::program_options::value<string>(), "options: full or webservice (defaults to full)")
    ("formats,p",       boost::program_options::value<string>(), "options: html,json,xml")
    ("force,f", "overwrite existing files without asking");
}

void New::generate_build2_files()
{
  generate_file("manifest");
  generate_file("buildfile");
  generate_file("app/buildfile");
  generate_file("config/buildfile");
  generate_file("repositories.manifest");
}

bool New::generate_project_structure()
{
  auto renderers = configuration.renderers();

  if (build_system == "build2")
    generate_build2_files();
  else
    generate_file("CMakeLists.txt");
  generate_file("app/main.cpp");
  generate_file("config/environment.cpp");
  generate_file("config/logger.cpp");
  generate_file("config/renderers.cpp");
  generate_file("config/request_pipe.cpp");
  generate_file("config/session_store.cpp");
  generate_file("spec/main.cpp");
  if (Crails::cast(vars, "with_action", false))
    generate_file("app/routes.cpp");
  if (Crails::cast(vars, "with_cookies", false))
    generate_file("config/salt.cpp");
  if (find(renderers.begin(), renderers.end(), "html") != renderers.end())
    generate_file("app/views/exception.html");
  configuration.save();
  return true;
}

int New::run()
{
  if (options.count("name"))
  {
    project_name = options["name"].as<string>();
    if (options.count("toolchain"))
      build_system = options["toolchain"].as<string>();
    if (options.count("configuration"))
      configuration_type = options["configuration"].as<string>();
    if (options.count("formats"))
      formats = Crails::split(options["formats"].as<string>(), ',');
    if (options.count("force") != 0)
      renderer.should_overwrite = true;
    if (validate_options() && move_to_project_directory())
    {
      configuration.version(LIBCRAILS_VERSION_STR);
      configuration.toolchain(build_system);
      vars["project_name"] = project_name;
      vars["configuration_type"] = configuration_type;
      vars["formats"] = &formats;
      use_actions(configuration_type == "full" || configuration_type == "webservice");
      use_cookies(configuration_type == "full");
      prepare_renderers();
      prepare_request_pipeline();
      return generate_project_structure();
    }
  }
  return -1;
}

bool New::validate_options()
{
  vector<string> available_configuration_types{"full","webservice","barebone"};
  vector<string> available_toolchains{"cmake", "build2"};
  regex validate_name("^[a-z0-9_\\-+\\.]{2}[a-z0-9_\\-+\\.]*$", regex_constants::icase);

  if (sregex_iterator(project_name.begin(), project_name.end(), validate_name) == sregex_iterator())
  {
    cerr << "Invalid project name." << endl;
    return false;
  }
  if (find(available_configuration_types.begin(), available_configuration_types.end(), configuration_type) == available_configuration_types.end())
  {
    cerr << "Invalid configuration `" << configuration_type << "`. Options are: " << Crails::join(available_configuration_types, ',') << '.' << endl;
    return false;
  }
  if (find(available_toolchains.begin(), available_toolchains.end(), build_system) == available_toolchains.end())
  {
    cerr << "Invalid toolchain `" << build_system << "`. Options are: " << Crails::join(available_toolchains, ',') << '.' << endl;
    return false;
  }
  return true;
}

void New::prepare_renderers()
{
  if (find(formats.begin(), formats.end(), "html") != formats.end())
  {
    project_parsers.push_back({"html_renderer", "HtmlRenderer"});
    configuration.add_renderer("html");
  }
  if (find(formats.begin(), formats.end(), "json") != formats.end())
  {
    project_parsers.push_back({"json_renderer", "JsonRenderer"});
    configuration.add_renderer("json");
  }
}

void New::prepare_request_pipeline()
{
  project_handlers.push_back({"file", "FileRequestHandler"});
  if (configuration_type == "full" || configuration_type == "webservice")
    project_handlers.push_back({"action", "ActionRequestHandler"});
  if (configuration_type == "full")
  {
    project_parsers.push_back({"form_parser", "RequestFormParser"});
    project_parsers.push_back({"url_parser", "RequestUrlParser"});
    project_parsers.push_back({"multipart_parser", "RequestMultipartParser"});
  }
  if (find(formats.begin(), formats.end(), "json") != formats.end())
  {
    project_parsers.push_back({"json_parser", "RequestJsonParser"});
  }
  if (find(formats.begin(), formats.end(), "xml") != formats.end())
  {
    project_parsers.push_back({"xml_parser", "RequestXmlParser"});
  }
}

bool New::move_to_project_directory()
{
  boost::filesystem::path path(target().data());

  if (!boost::filesystem::is_directory(path))
  {
    boost::system::error_code ec;

    boost::filesystem::create_directories(path, ec);
    if (ec)
    {
      cout << "Failed to reach project directory: " << ec.message() << endl;
      return false;
    }
  }
  boost::filesystem::current_path(path);
  return true;
}
