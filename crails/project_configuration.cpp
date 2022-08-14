#include "project_configuration.hpp"
#include "render_file.hpp"
#include "read_file.hpp"
#include "utils/string.hpp"
#include <boost/filesystem.hpp>
#include "version.hpp"

using namespace std;

ProjectConfiguration::ProjectConfiguration()
{
}

void ProjectConfiguration::initialize()
{
  string       configuration_contents;
  list<string> configuration_lines;

  move_to_project_directory();
  Crails::read_file(".crails", configuration_contents);
  configuration_lines = Crails::split(configuration_contents, '\n');
  for (auto configuration_line : configuration_lines)
  {
    auto parts = Crails::split(configuration_line, ':');

    if (parts.size() == 2)
      variables.emplace(*parts.begin(), *parts.rbegin());
  }
}

void ProjectConfiguration::save()
{
  Crails::RenderFile target;
  stringstream stream;

  for (auto it = variables.begin() ; it != variables.end() ; ++it)
    stream << it->first << ':' << it->second << '\n';
  target.open(".crails");
  target.set_body(stream.str().c_str(), stream.str().length());
}

string ProjectConfiguration::crails_bin_path() const
{
  const static boost::filesystem::path path = boost::filesystem::canonical("/proc/self/exe");

  return path.parent_path().string();
}

string ProjectConfiguration::version() const
{
  auto it = variables.find("crails-version");
  return it == variables.end() ? string(LIBCRAILS_VERSION_STR) : string(it->second);
}

void ProjectConfiguration::version(const string& version)
{
  variables["crails-version"] = version;
}


string ProjectConfiguration::toolchain() const
{
  auto it = variables.find("build_system");
  return it == variables.end() ? string("cmake") : string(it->second);
}

void ProjectConfiguration::toolchain(const string& value)
{
  variables["build_system"] = value;
}

list<string> ProjectConfiguration::renderers() const
{
  auto it = variables.find("renderers");

  return it == variables.end() ? list<string>() : Crails::split(it->second, ',');
}

void ProjectConfiguration::renderers(const list<string>& value)
{
  variables["renderers"] = Crails::join(value, ',');
}

void ProjectConfiguration::add_renderer(const string& value)
{
  auto renderer_list = renderers();
  if (find(renderer_list.begin(), renderer_list.end(), value) == renderer_list.end())
    renderer_list.push_back(value);
  renderers(renderer_list);
}

void ProjectConfiguration::remove_renderer(const string& value)
{
  auto renderer_list = renderers();
  auto it = find(renderer_list.begin(), renderer_list.end(), value);
  if (it != renderer_list.end())
  {
    renderer_list.erase(it);
    renderers(renderer_list);
  }
}

string ProjectConfiguration::project_directory()
{
  auto path = boost::filesystem::canonical(boost::filesystem::current_path());

  while (!boost::filesystem::exists(path.string() + "/.crails"))
  {
    if (path == path.parent_path())
      throw std::runtime_error("Not in a Crails project.");
    path = path.parent_path();
  }
  return path.string();
}

void ProjectConfiguration::move_to_project_directory()
{
  boost::filesystem::current_path(project_directory());
}
