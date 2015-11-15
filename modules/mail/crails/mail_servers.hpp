#ifndef  MAIL_SERVERS_HPP
# define MAIL_SERVERS_HPP

# include <crails/utils/singleton.hpp>
# include <crails/smtp.hpp>
# include <crails/datatree.hpp>
# include <map>

namespace Crails
{
  class MailServers
  {
    SINGLETON(MailServers)
  public:
    void configure_mail_server(const std::string& conf_name, Smtp::Server& server) const;

  private:
    class Conf
    {
    public:
      Conf(Data server_data);

      void connect_server(Smtp::Server& server) const;

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

    void load_mail_servers(Data data);

    ServerConfs server_confs;
    DataTree    data_tree;
  };
}

#endif
