#pragma once
#include "../file_renderer.hpp"
#include "../file_editor.hpp"
#include "utils/string.hpp"

class ControllerScaffold
{
  FileRenderer renderer;
  std::string classname;
  std::string path_name;
  std::string target_folder = "app/controllers";
public:
  void options_description(boost::program_options::options_description& desc) const
  {
    desc.add_options()
      ("name,n",   boost::program_options::value<std::string>(), "classname")
      ("target,t", boost::program_options::value<std::string>(), "target folder (defaults to `app/controllers`)")
      ("mode,m",   boost::program_options::value<std::string>(), "generates methods and routes `crud` or `resource`");
  }

  int create(boost::program_options::variables_map& options)
  {
    if (!options.count("name"))
    {
      std::cerr << "Option --name required" << std::endl;
      return -1;
    }
    classname = options["name"].as<std::string>();
    if (options.count("target"))
      target_folder = options["target"].as<std::string>();
    renderer.vars["classname"] = classname;
    path_name = Crails::underscore(classname);
    if (options.count("mode"))
      renderer.vars["crud_scaffold"] = options["mode"].as<std::string>() == "crud";
    if (options.count("mode"))
      renderer.vars["resource_scaffold"] = options["mode"].as<std::string>() == "resource";
    renderer.generate_file(
      "scaffolds/controller.cpp",
      target_folder + '/' + path_name + ".cpp"
    );
    renderer.generate_file(
      "scaffolds/controller.hpp",
      target_folder + '/' + path_name + ".hpp"
    );
    if (options.count("mode"))
      update_router(options);
    return 0;
  }

  void update_router(boost::program_options::variables_map& options)
  {
    CrailsFileEditor router("app/routes.cpp", "Append routes here");

    if (router.load_file())
    {
      if (options["mode"].as<std::string>() == "crud")
        router.insert("  crud_actions(" + path_name + ", " + classname + ");\n");
      else if (options["mode"].as<std::string>() == "resource")
        router.insert("  resource_actions(" + path_name + ", " + classname + ");\n");
      router.add_include(target_folder + '/' + path_name + ".hpp");
      router.save_file();
    }
    else
      std::cout << "Cannot update app/routes.cpp" << std::endl;
  }
};
