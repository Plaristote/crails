#ifndef  MAILER_HPP
# define MAILER_HPP

# include "mail_servers.hpp"
# include "controller.hpp"

namespace Crails
{
  class Mailer
  {
  public:
    Mailer(Controller& controller, const std::string& configuration);

  protected:
    Smtp::Mail   mail;
    SharedVars&  vars;
    Params&      params;

    void render(const std::string& view);
    void send(void);
    
  private:
    Controller&  controller;
    std::string  configuration;
    Smtp::Server smtp_server;
    bool         is_connected;
  };
}

#endif
