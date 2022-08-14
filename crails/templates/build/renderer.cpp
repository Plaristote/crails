#include <sstream>
#include "shared_vars.hpp"
#include "template.hpp"
#include "utils/string.hpp"
using namespace std;

class RendererCpp : public Crails::Template
{
public:
  RendererCpp(const Crails::Renderer* renderer, Crails::SharedVars& vars) :
    Crails::Template(renderer, vars), 
    renderer_name(Crails::cast<string>(vars, "renderer_name")), 
    classname( Crails::uppercase(renderer_name) + "Renderer"), 
    targets(reinterpret_cast<map<string, string>&>(*Crails::cast<map<string, string>*>(vars, "targets")))
  {}

  std::string render()
  {
ecpp_stream << "#include <crails/renderers/" << ( renderer_name );
  ecpp_stream << "_renderer.hpp>\n#define declare_renderer(name) std::string render_##name(const Crails::Renderer*, Crails::SharedVars&)\n#define add_renderer(path, name) templates.insert(std::pair<std::string, Generator>(path, render_##name))\n";
 for (auto it = targets.begin() ; it != targets.end() ; ++it){
  ecpp_stream << "\ndeclare_renderer(" << ( it->second );
  ecpp_stream << ");";
 };
  ecpp_stream << "\n\nCrails::" << ( classname );
  ecpp_stream << "::" << ( classname );
  ecpp_stream << "()\n{";
 for (auto it = targets.begin() ; it != targets.end() ; ++it){
  ecpp_stream << "\n  add_renderer(\"" << ( it->first );
  ecpp_stream << "\", " << ( it->second );
  ecpp_stream << ");";
 };
  ecpp_stream << "\n}\n";
    return ecpp_stream.str();
  }
private:
  std::stringstream ecpp_stream;
  string renderer_name;
  string classname;
  map<string, string>& targets;
};

std::string render_renderer_cpp(const Crails::Renderer* renderer, Crails::SharedVars& vars)
{
  return RendererCpp(renderer, vars).render();
}