#include <sstream>
#include "shared_vars.hpp"
#include "template.hpp"

class ProjectAppMainCpp : public Crails::Template
{
public:
  ProjectAppMainCpp(const Crails::Renderer* renderer, Crails::SharedVars& vars) :
    Crails::Template(renderer, vars), 
    with_action(Crails::cast<bool>(vars, "with_action",  true)), 
    with_cookies(Crails::cast<bool>(vars, "with_cookies",  false))
  {}

  std::string render()
  {
ecpp_stream << "#include <crails/server.hpp>\n#include <crails/renderer.hpp>";
 if (with_action){
  ecpp_stream << "\n#include <crails/router.hpp>";
 };
  ecpp_stream << "";
 if (with_cookies){
  ecpp_stream << "#include <crails/cipher.hpp>\n#include <crails/cookie_data.hpp>";
 };
  ecpp_stream << "\n\nusing namespace std;\nusing namespace Crails;\n\nint main(int argc, const char **argv)\n{";
 if (with_action){
  ecpp_stream << "\n  SingletonInstantiator<Router> router;\n\n  router->initialize();";
 };
  ecpp_stream << "\n  // Initializers\n  Renderer::initialize();";
 if (with_cookies){
  ecpp_stream << "\n  if (CookieData::use_encryption)\n    Cipher::initialize();";
 };
  ecpp_stream << "\n  // Application loop\n  Server::launch(argc, argv);\n\n  // Finalizers\n  Renderer::finalize();\n  return (0);\n}\n";
    return ecpp_stream.str();
  }
private:
  std::stringstream ecpp_stream;
  bool with_action;
  bool with_cookies;
};

std::string render_project_app_main_cpp(const Crails::Renderer* renderer, Crails::SharedVars& vars)
{
  return ProjectAppMainCpp(renderer, vars).render();
}