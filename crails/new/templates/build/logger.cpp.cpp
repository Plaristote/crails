#include <sstream>
#include "shared_vars.hpp"
#include "template.hpp"

class ProjectConfigLoggerCpp : public Crails::Template
{
public:
  ProjectConfigLoggerCpp(const Crails::Renderer* renderer, Crails::SharedVars& vars) :
    Crails::Template(renderer, vars)
  {}

  std::string render()
  {
ecpp_stream << "#include <crails/logger.hpp>\n\nusing namespace Crails;\n\nconst Logger::Symbol Logger::log_level = Info;\n";
    return ecpp_stream.str();
  }
private:
  std::stringstream ecpp_stream;
};

std::string render_project_config_logger_cpp(const Crails::Renderer* renderer, Crails::SharedVars& vars)
{
  return ProjectConfigLoggerCpp(renderer, vars).render();
}