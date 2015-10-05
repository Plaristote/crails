#include "crails/mailer.hpp"

using namespace std;
using namespace Crails;

Mailer::Mailer(Controller& controller, const std::string& configuration) : vars(controller.vars), params(controller.params), controller(controller), configuration(configuration)
{
  is_connected = false;
}

void Mailer::render(const std::string& view)
{
  std::string backup = controller.response["body"].Value();

  controller.render(view);
  mail.SetBody(controller.response["body"].Value());
  controller.response["body"] = backup;
}

void Mailer::send(void)
{
  if (!is_connected)
  {
    MailServers::singleton::Get()->ConfigureMailServer(configuration, smtp_server);
    is_connected = true;
  }
  smtp_server.Send(mail);
}
