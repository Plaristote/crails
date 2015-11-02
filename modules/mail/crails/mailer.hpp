#ifndef  MAILER_HPP
# define MAILER_HPP

# include <crails/controller.hpp>
# include <memory>
# include "mail_servers.hpp"

namespace Crails
{
  class Mailer
  {
  public:
    Mailer(Controller& controller, const std::string& configuration);
    Mailer(const std::string& configuration);

    void render(const std::string& view);
    void send(void);

  protected:
    Smtp::Mail   mail;
    SharedVars   vars;

  private:
    Controller*   controller;
    std::string   configuration;
    Smtp::Server  smtp_server;
    bool          is_connected;
  };
}

#endif
