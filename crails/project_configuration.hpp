#pragma once
#include <map>
#include <list>
#include <string>

class ProjectConfiguration
{
  std::map<std::string, std::string> variables;
public:
  ProjectConfiguration();

  static void        create();
  static std::string project_directory();
  static void        move_to_project_directory();

  void initialize();
  void save();

  std::string version() const;
  void        version(const std::string&);

  std::string toolchain() const;
  void        toolchain(const std::string&);

  std::list<std::string> renderers() const;
  void                   renderers(const std::list<std::string>&);
  void                   add_renderer(const std::string&);
  void                   remove_renderer(const std::string&);

  std::string crails_bin_path() const;
};
