#include <sstream>
#include "shared_vars.hpp"
#include "template.hpp"

class ProjectSpecMainCpp : public Crails::Template
{
public:
  ProjectSpecMainCpp(const Crails::Renderer* renderer, Crails::SharedVars& vars) :
    Crails::Template(renderer, vars), 
    configuration_type(Crails::cast<std::string>(vars, "configuration_type")), 
    has_router( configuration_type == "webservice" || configuration_type == "full")
  {}

  std::string render()
  {
ecpp_stream << "#include <crails/tests/runner.hpp>";
 if (has_router){
  ecpp_stream << "\n#include <crails/router.hpp>";
 };
  ecpp_stream << "\n\nvoid Crails::Tests::Runner::setup()\n{";
 if (has_router){
  ecpp_stream << "\n  Router::singleton::Initialize();\n  Router::singleton::Get()->initialize();";
 };
  ecpp_stream << "\n}\n\nvoid Crails::Tests::Runner::shutdown()\n{";
 if (has_router){
  ecpp_stream << "\n  Router::singleton::Finalize();";
 };
  ecpp_stream << "\n}\n";
    return ecpp_stream.str();
  }
private:
  std::stringstream ecpp_stream;
  std::string configuration_type;
  bool has_router;
};

std::string render_project_spec_main_cpp(const Crails::Renderer* renderer, Crails::SharedVars& vars)
{
  return ProjectSpecMainCpp(renderer, vars).render();
}