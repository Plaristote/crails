#include <sstream>
#include "shared_vars.hpp"
#include "template.hpp"
#include "utils/string.hpp"
using namespace std;

class ScaffoldsControllerCpp : public Crails::Template
{
public:
  ScaffoldsControllerCpp(const Crails::Renderer* renderer, Crails::SharedVars& vars) :
    Crails::Template(renderer, vars), 
    classname(Crails::cast<string>(vars, "classname")), 
    parent_class(Crails::cast<string>(vars, "parent_class",  "ApplicationController")), 
    path( Crails::underscore(classname)), 
    resource_scaffold(Crails::cast<bool>(vars, "resource_scaffold",  false)), 
    crud_scaffold(Crails::cast<bool>(vars, "crud_scaffold",  false))
  {}

  std::string render()
  {
ecpp_stream << "#include \"" << ( path );
  ecpp_stream << ".hpp\"\n" << ( classname + "::" + classname );
  ecpp_stream << "(Crails::Context& context) : " << ( parent_class );
  ecpp_stream << "(context)\n{\n}\n\nvoid " << ( classname );
  ecpp_stream << "::initialize()\n{\n}\n\nvoid " << ( classname );
  ecpp_stream << "::finalize()\n{\n}\n";
 if (resource_scaffold || crud_scaffold){
  ecpp_stream << "\nvoid " << ( classname );
  ecpp_stream << "::index()\n{\n  render(\"" << ( path );
  ecpp_stream << "/index\");\n}\n\nvoid " << ( classname );
  ecpp_stream << "::show()\n{\n  render(\"" << ( path );
  ecpp_stream << "/show\");\n}\n\nvoid " << ( classname );
  ecpp_stream << "::create()\n{\n  render(\"" << ( path );
  ecpp_stream << "/show\");\n}\n\nvoid " << ( classname );
  ecpp_stream << "::update()\n{\n  render(\"" << ( path );
  ecpp_stream << "/show\");\n}\n\nvoid " << ( classname );
  ecpp_stream << "::destroy()\n{\n  redirect_to(\"" << ( path );
  ecpp_stream << "\");\n}";
 if (resource_scaffold){
  ecpp_stream << "\nvoid " << ( classname );
  ecpp_stream << "::new_()\n{\n  render(\"" << ( path );
  ecpp_stream << "/new\");\n}\n\nvoid " << ( classname );
  ecpp_stream << "::edit()\n{\n  render(\"" << ( path );
  ecpp_stream << "/edit\");\n}";
 };
  ecpp_stream << "";
 };
  ecpp_stream << "";
    return ecpp_stream.str();
  }
private:
  std::stringstream ecpp_stream;
  string classname;
  string parent_class;
  string path;
  bool resource_scaffold;
  bool crud_scaffold;
};

std::string render_scaffolds_controller_cpp(const Crails::Renderer* renderer, Crails::SharedVars& vars)
{
  return ScaffoldsControllerCpp(renderer, vars).render();
}