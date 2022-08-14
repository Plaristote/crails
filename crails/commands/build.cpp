#include "build.hpp"
#include "utils/string.hpp"
#include <boost/process.hpp>
#include <boost/filesystem.hpp>

using namespace std;

bool BuildManager::prebuild_renderers()
{
  std::cout << "prebuild renderers " << configuration.renderers().size() << std::endl;
  for (const auto& renderer : configuration.renderers())
  {
    stringstream command;

    command << configuration.crails_bin_path() + "/crails"
      << " templates build -r " << renderer << " -i app/views "
      << " -t Crails::" << Crails::uppercase(renderer) << "Template"
      << " -z crails/" << renderer << "_template.hpp";
    boost::process::child process(command.str());
    process.wait();
    if (process.exit_code() != 0)
      return false;
  }
  return true;
}

bool BuildManager::build_with_cmake()
{
  boost::filesystem::path current_dir(configuration.project_directory() + "/build");
  boost::process::child cmake("cmake " + configuration.project_directory());

  cmake.wait();
  if (cmake.exit_code() == 0)
  {
    boost::process::child make("make");

    make.wait();
    return make.exit_code() == 0;
  }
  return false;
}

int BuildManager::run()
{
  configuration.initialize();
  prebuild_renderers();
  if (configuration.toolchain() == "cmake")
    return build_with_cmake() ? 0 : -1;
  else
    cerr << "Build command not supported for " << configuration.toolchain() << endl;
  return -1;
}
