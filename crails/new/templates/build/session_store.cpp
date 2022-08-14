#include <sstream>
#include "shared_vars.hpp"
#include "template.hpp"
#include "utils/string.hpp"

class ProjectConfigSessionStoreCpp : public Crails::Template
{
public:
  ProjectConfigSessionStoreCpp(const Crails::Renderer* renderer, Crails::SharedVars& vars) :
    Crails::Template(renderer, vars), 
    session_store(Crails::cast<std::string>(vars, "session_store",  "NoSessionStore"))
  {}

  std::string render()
  {
ecpp_stream << "#include <crails/session_store/" << ( Crails::underscore(session_store) );
  ecpp_stream << ".hpp>\n#include <crails/session_store.hpp>\n\nusing namespace Crails;\n\nUSE_SESSION_STORE(" << ( session_store );
  ecpp_stream << ")\n";
    return ecpp_stream.str();
  }
private:
  std::stringstream ecpp_stream;
  std::string session_store;
};

std::string render_project_config_session_store_cpp(const Crails::Renderer* renderer, Crails::SharedVars& vars)
{
  return ProjectConfigSessionStoreCpp(renderer, vars).render();
}