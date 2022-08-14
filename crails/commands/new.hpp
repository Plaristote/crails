#pragma once
#include "../command.hpp"
#include "../file_renderer.hpp"
#include "template.hpp"

class New : public Crails::Command
{
  typedef std::pair<std::string, std::string> IncludeAndConstructor;
  typedef std::vector<IncludeAndConstructor> IncludeAndConstructors;

  FileRenderer             renderer;
  Crails::SharedVars&      vars;
  IncludeAndConstructors   project_renderers;
  IncludeAndConstructors   project_parsers;
  IncludeAndConstructors   project_handlers;
  std::string              project_name;
  std::string              build_system = "cmake";
  std::string              configuration_type = "full";
  std::list<std::string>   formats;
public:
  New();

  std::string_view description() const override { return "Creates a new Crails application."; }
  void             options_description(boost::program_options::options_description& desc) const override;
  int              run() override;
private:
  void             generate_build2_files();
  bool             generate_project_structure();
  bool             generate_file(std::string_view filepath) { return renderer.generate_file(filepath, filepath); }

  void             prepare_renderers();
  void             prepare_request_pipeline();
  bool             validate_options();
  void             use_actions(bool value) { vars["with_action"] = value; }
  void             use_cookies(bool value) { vars["with_cookies"] = value; }

  std::string_view target() const { return (options.count("target") ? options["target"] : options["name"]).as<std::string>(); }
  bool             move_to_project_directory();
};
