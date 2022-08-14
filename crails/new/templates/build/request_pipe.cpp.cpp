#include <sstream>
#include "shared_vars.hpp"
#include "template.hpp"
using namespace std;

class ProjectConfigRequestPipeCpp : public Crails::Template
{
public:
  ProjectConfigRequestPipeCpp(const Crails::Renderer* renderer, Crails::SharedVars& vars) :
    Crails::Template(renderer, vars), 
    parsers(reinterpret_cast<vector<pair<string, string>>&>(*Crails::cast<vector<pair<string, string>>*>(vars, "parsers"))), 
    handlers(reinterpret_cast<vector<pair<string, string>>&>(*Crails::cast<vector<pair<string, string>>*>(vars, "handlers")))
  {}

  std::string render()
  {
ecpp_stream << "";
 for (auto parser : parsers){
  ecpp_stream << "" << ( "#include <crails/request_parsers/" + parser.first + ".hpp>\n" );
  ecpp_stream << "";
 };
  ecpp_stream << "";
 for (auto handler : handlers){
  ecpp_stream << "" << ( "#include <crails/request_handlers/" + handler.first + ".hpp>\n" );
  ecpp_stream << "";
 };
  ecpp_stream << "#include <crails/request_handlers/file.hpp>\n#include <crails/request_handlers/action.hpp>\n#include <crails/request_parser.hpp>\n\nusing namespace Crails;\n\nconst std::string Server::temporary_path = \"/tmp\";\n\nvoid Server::initialize_request_pipe()\n{";
 for (auto parser : parsers){
  ecpp_stream << "\n  " << ( "add_request_parser(new " + parser.second + ");" );
  ecpp_stream << "";
 };
  ecpp_stream << "";
 for (auto handler : handlers){
  ecpp_stream << "\n  " << ( "add_request_handler(new " + handler.second + ");" );
  ecpp_stream << "";
 };
  ecpp_stream << "\n}\n";
    return ecpp_stream.str();
  }
private:
  std::stringstream ecpp_stream;
  vector<pair<string, string>>& parsers;
  vector<pair<string, string>>& handlers;
};

std::string render_project_config_request_pipe_cpp(const Crails::Renderer* renderer, Crails::SharedVars& vars)
{
  return ProjectConfigRequestPipeCpp(renderer, vars).render();
}