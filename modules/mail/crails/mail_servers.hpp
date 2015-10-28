#ifndef  MAIL_SERVERS_HPP
# define MAIL_SERVERS_HPP

# include <Boots/Utils/singleton.hpp>
# include <crails/smtp.hpp>
# include <crails/datatree.hpp>
# include <crails/smtp.hpp>
# include <map>

namespace Crails
{
  class MailServers
  {
    SINGLETON(MailServers)
  public:
    void ConfigureMailServer(const std::string& conf_name, Smtp::Server& server) const;

  private:
    class Conf
    {
    public:
      Conf(Data server_data);

      void ConnectServer(Smtp::Server& server) const;

    private:
      std::string                          hostname;
      unsigned short                       port;
      bool                                 use_authentication, use_tls;
      Smtp::Server::AuthenticationProtocol authentication_protocol;
      std::string                          username;
      std::string                          password;
    };

    typedef std::map<std::string, Conf> ServerConfs;
    
    MailServers(void);

    void LoadMailServers(Data data);

    ServerConfs server_confs;
  };
}

#endif
