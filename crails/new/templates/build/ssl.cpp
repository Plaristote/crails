#include <sstream>
#include "shared_vars.hpp"
#include "template.hpp"

class ProjectConfigSslCpp : public Crails::Template
{
public:
  ProjectConfigSslCpp(const Crails::Renderer* renderer, Crails::SharedVars& vars) :
    Crails::Template(renderer, vars)
  {}

  std::string render()
  {
ecpp_stream << "#include <boost/asio.hpp>\n#include <boost/asio/ssl.hpp>\n\nusing namespace boost;\n\nnamespace Crails\n{\n  std::string get_ssl_password(std::size_t max_length, asio::ssl::context_base::password_purpose purpose)\n  {\n    return \"your_ssl_password\";\n  }\n}\n";
    return ecpp_stream.str();
  }
private:
  std::stringstream ecpp_stream;
};

std::string render_project_config_ssl_cpp(const Crails::Renderer* renderer, Crails::SharedVars& vars)
{
  return ProjectConfigSslCpp(renderer, vars).render();
}