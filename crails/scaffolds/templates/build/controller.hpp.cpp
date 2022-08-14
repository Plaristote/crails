#include <sstream>
#include "shared_vars.hpp"
#include "template.hpp"
using namespace std;

class ScaffoldsControllerHpp : public Crails::Template
{
public:
  ScaffoldsControllerHpp(const Crails::Renderer* renderer, Crails::SharedVars& vars) :
    Crails::Template(renderer, vars), 
    classname(Crails::cast<string>(vars, "classname")), 
    parent_class(Crails::cast<string>(vars, "parent_class",  "ApplicationController")), 
    parent_header(Crails::cast<string>(vars, "parent_header",  "app/controllers/application.hpp")), 
    resource_scaffold(Crails::cast<bool>(vars, "resource_scaffold",  false)), 
    crud_scaffold(Crails::cast<bool>(vars, "crud_scaffold",  false))
  {}

  std::string render()
  {
ecpp_stream << "#pragma once\n#include \"" << ( parent_header );
  ecpp_stream << "\"\n\nclass " << ( classname );
  ecpp_stream << " : public " << ( parent_class );
  ecpp_stream << "\n{\npublic:\n  " << ( classname );
  ecpp_stream << "(Crails::Context&);\n\n  void initialize();\n  void finalize();";
 if (resource_scaffold || crud_scaffold){
  ecpp_stream << "\n  void index();\n  void show();\n  void create();\n  void update();\n  void destroy();";
 if (resource_scaffold){
  ecpp_stream << "\n  void new_();\n  void edit();";
 };
  ecpp_stream << "";
 };
  ecpp_stream << "};\n";
    return ecpp_stream.str();
  }
private:
  std::stringstream ecpp_stream;
  string classname;
  string parent_class;
  string parent_header;
  bool resource_scaffold;
  bool crud_scaffold;
};

std::string render_scaffolds_controller_hpp(const Crails::Renderer* renderer, Crails::SharedVars& vars)
{
  return ScaffoldsControllerHpp(renderer, vars).render();
}