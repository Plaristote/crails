#include <crails/mailer.hpp>
#include <crails/renderer.hpp>

using namespace std;
using namespace Crails;

Mailer::Mailer(Controller& controller, const std::string& configuration) : controller(&controller), configuration(configuration), is_connected(false)
{
}

Mailer::Mailer(const std::string& configuration) : controller(0), configuration(configuration), is_connected(false)
{
}

void Mailer::render(const std::string& view)
{
  DataTree mail_params, mail_response;
  SharedVars& vars = controller ? controller->vars : this->vars;

  mail_params["headers"]["Accept"] = "text/html text/plain";
  Renderer::render(view, mail_params.as_data(), mail_response.as_data(), vars);
  mail.SetContentType(mail_response["headers"]["Content-Type"].defaults_to<string>(""));
  mail.SetBody(mail_response["body"].as<string>());
}

void Mailer::send(void)
{
  if (!is_connected)
  {
    MailServers::singleton::Get()->ConfigureMailServer(configuration, smtp_server);
    is_connected = true;
  }
  smtp_server.send(mail);
}
