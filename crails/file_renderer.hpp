#pragma once
#include "project_renderer.hpp"
#include "shared_vars.hpp"
#include <boost/filesystem.hpp>

class FileRenderer
{
public:
  ProjectRenderer    renderer;
  Crails::SharedVars vars;
  bool               should_overwrite = false;

  bool generate_file(std::string_view template_name, std::string_view filepath);
private:
  bool overwrite_prompt(std::string_view template_name, std::string_view local_path, boost::filesystem::path path);
  bool render_file(std::string_view template_name, boost::filesystem::path);
  bool require_folder(boost::filesystem::path);
};
