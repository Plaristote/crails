#include <sstream>
#include "shared_vars.hpp"
#include "template.hpp"
#include <algorithm>
#include <list>

class ProjectCmakelistsTxt : public Crails::Template
{
public:
  ProjectCmakelistsTxt(const Crails::Renderer* renderer, Crails::SharedVars& vars) :
    Crails::Template(renderer, vars), 
    project_name(Crails::cast<std::string>(vars, "project_name")), 
    configuration_type(Crails::cast<std::string>(vars, "configuration_type",  "full")), 
    formats(reinterpret_cast<std::list<std::string>&>(*Crails::cast<std::list<std::string>*>(vars, "formats")))
  {}

  std::string render()
  {
ecpp_stream << "cmake_minimum_required(VERSION 3.0)\n\nproject(" << ( project_name );
  ecpp_stream << ")\n\nset(CMAKE_CXX_FLAGS \"-std=c++17 -Wall -Wno-unknown-pragmas -Wno-deprecated-declarations -pedantic \")\n\nfind_package(Boost COMPONENTS thread filesystem program_options system random REQUIRED)\n\ninclude_directories(include /usr/local/include ${Boost_INCLUDE_DIRS} .)\n\nfile(GLOB_RECURSE crails_app\n  app/controllers/*.cpp\n  app/models/*.cpp\n  app/models/*.cxx\n  app/views/*.cpp\n  app/routes.cpp\n  config/*.cpp\n  lib/*.cpp\n  lib/*.cxx\n  modules/*.cpp\n  modules/*.cxx)\n\nfile(GLOB_RECURSE server_files\n     app/main.cpp)\n\nfile(GLOB_RECURSE tests_files\n     spec/*.cpp)\n\n# Add your modules here (do not modify this line)";
 if (configuration_type == "full" || configuration_type == "webservice"){
  ecpp_stream << "\nset(dependencies ${dependencies} libcrails-action)";
 };
  ecpp_stream << "";
 if (configuration_type == "full"){
  ecpp_stream << "set(dependencies ${dependencies} \\n  libcrails-cookies libcrails-databases \\n  libcrails-url-parser libcrails-form-parser libcrails-multipart-parser)";
 };
  ecpp_stream << "";
 if (std::find(formats.begin(), formats.end(), "json") != formats.end()){
  ecpp_stream << "set(dependencies ${dependencies} libcrails-json-parser libcrails-json-views)";
 };
  ecpp_stream << "";
 if (std::find(formats.begin(), formats.end(), "xml") != formats.end()){
  ecpp_stream << "set(dependencies ${dependencies} libcrails-xml-parser)";
 };
  ecpp_stream << "";
 if (std::find(formats.begin(), formats.end(), "html") != formats.end()){
  ecpp_stream << "set(dependencies ${dependencies} libcrails-html-views)";
 };
  ecpp_stream << "\nadd_library(crails-app SHARED ${crails_app})\nadd_executable(server ${server_files})\nadd_executable(tests  ${tests_files})\n\nset(dependencies libcrails\n                 ${Boost_LIBRARIES}\n                 pthread dl crypto ssl\n                 ${dependencies}\n                 )\n\n# Custom dependencies (do not modify this line)\n\ntarget_link_libraries(server ${dependencies})\ntarget_link_libraries(tests  libcrails-tests ${dependencies} ${tests_dependencies})\n";
    return ecpp_stream.str();
  }
private:
  std::stringstream ecpp_stream;
  std::string project_name;
  std::string configuration_type;
  std::list<std::string>& formats;
};

std::string render_project_cmakelists_txt(const Crails::Renderer* renderer, Crails::SharedVars& vars)
{
  return ProjectCmakelistsTxt(renderer, vars).render();
}