#include <sstream>
#include "shared_vars.hpp"
#include "template.hpp"

class ProjectConfigEnvironmentCpp : public Crails::Template
{
public:
  ProjectConfigEnvironmentCpp(const Crails::Renderer* renderer, Crails::SharedVars& vars) :
    Crails::Template(renderer, vars), 
    environment(Crails::cast<std::string>(vars, "environment",  "Development"))
  {}

  std::string render()
  {
ecpp_stream << "#include <crails/environment.hpp>\n\nnamespace Crails\n{\n  Environment environment = " << ( environment );
  ecpp_stream << ";\n}\n";
    return ecpp_stream.str();
  }
private:
  std::stringstream ecpp_stream;
  std::string environment;
};

std::string render_project_config_environment_cpp(const Crails::Renderer* renderer, Crails::SharedVars& vars)
{
  return ProjectConfigEnvironmentCpp(renderer, vars).render();
}