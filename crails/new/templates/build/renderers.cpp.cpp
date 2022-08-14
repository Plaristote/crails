#include <sstream>
#include "shared_vars.hpp"
#include "template.hpp"
using namespace std;

class ProjectConfigRenderersCpp : public Crails::Template
{
public:
  ProjectConfigRenderersCpp(const Crails::Renderer* renderer, Crails::SharedVars& vars) :
    Crails::Template(renderer, vars), 
    default_format(Crails::cast<string>(vars, "default_format",  "text/html")), 
    renderers(reinterpret_cast<vector<pair<string, string>>&>(*Crails::cast<vector<pair<string, string>>*>(vars, "renderers")))
  {}

  std::string render()
  {
ecpp_stream << "#include <crails/renderer.hpp>";
 for (auto renderer : renderers){
  ecpp_stream << "" << ( "#include <crails/renderers/" + renderer.first + ".hpp>\n" );
  ecpp_stream << "";
 };
  ecpp_stream << "\n\nusing namespace Crails;\n\nconst std::string Renderer::default_format = \"" << ( default_format );
  ecpp_stream << "\";\n\nvoid Renderer::initialize()\n{\n  // Append renderers";
 for (auto renderer : renderers){
  ecpp_stream << "\n  " << ( "renderers.push_back(new " + renderer.second + ");\n" );
  ecpp_stream << "";
 };
  ecpp_stream << "\n}\n";
    return ecpp_stream.str();
  }
private:
  std::stringstream ecpp_stream;
  string default_format;
  vector<pair<string, string>>& renderers;
};

std::string render_project_config_renderers_cpp(const Crails::Renderer* renderer, Crails::SharedVars& vars)
{
  return ProjectConfigRenderersCpp(renderer, vars).render();
}