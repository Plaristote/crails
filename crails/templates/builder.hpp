#pragma once
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <map>
#include "../command.hpp"

namespace Crails { class RenderFile; }

class TemplateBuilder : public Crails::Command
{
  struct Target { std::string alias, classname, function; };
  typedef std::map<std::string, Target> Targets;

  std::string renderer;
  std::string pattern = "\\.ecpp$";
  std::string output_directory = "lib/renderers";
  Targets     targets;
public:
  int              run() override;
  std::string_view description() const override { return "compile templates to C++ for a given renderer"; }
  void             options_description(boost::program_options::options_description& desc) const override;
  void             collect_files();
  void             collect_files(boost::filesystem::path directory);
  bool             validate_options();
  bool             generate_templates();
  bool             generate_renderer_ctor();
  std::string      command_for_target(const std::pair<std::string, Target>&) const;
  bool             run_ecpp(const std::pair<std::string, Target>&, Crails::RenderFile& output) const;

  void clear_dropped_templates();
  void prune_up_to_date_template(Targets::iterator, boost::filesystem::path existing_template);
};
