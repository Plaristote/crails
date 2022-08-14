#include <sstream>
#include "shared_vars.hpp"
#include "template.hpp"

class ProjectConfigDatabasesCpp : public Crails::Template
{
public:
  ProjectConfigDatabasesCpp(const Crails::Renderer* renderer, Crails::SharedVars& vars) :
    Crails::Template(renderer, vars)
  {}

  std::string render()
  {
ecpp_stream << "#include <crails/databases.hpp>\n\nusing namespace Crails;\n\nconst Databases::Settings Databases::settings = {\n  {\n    \"production\", {\n    }\n  },\n\n  {\n    \"development\", {\n    }\n  },\n\n  {\n    \"test\", {\n    }\n  }\n};\n";
    return ecpp_stream.str();
  }
private:
  std::stringstream ecpp_stream;
};

std::string render_project_config_databases_cpp(const Crails::Renderer* renderer, Crails::SharedVars& vars)
{
  return ProjectConfigDatabasesCpp(renderer, vars).render();
}