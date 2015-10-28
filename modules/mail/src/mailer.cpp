#include <crails/mailer.hpp>
#include <crails/renderer.hpp>

using namespace std;
using namespace Crails;

Mailer::Mailer(Controller& controller, const std::string& configuration) : vars(controller.vars), params(controller.params), controller(controller), configuration(configuration)
{
  is_connected = false;
}

void Mailer::render(const std::string& view)
{
  DynStruct mail_params, mail_response;

  mail_params["headers"]["Accept"] = "text/html text/plain";
  Renderer::render(view, mail_params, mail_response, controller.vars);
  mail.SetBody(mail_response["body"].Value());
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
