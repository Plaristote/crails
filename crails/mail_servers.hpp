#ifndef  MAIL_SERVERS_HPP
# define MAIL_SERVERS_HPP

# include <Boots/Network/smtp.hpp>
# include <Boots/Utils/singleton.hpp>
# include <Boots/Utils/datatree.hpp>
# include <map>

namespace Crails
{
  class MailServers
  {
    SINGLETON(MailServers)
  public:
    void ConfigureMailServer(const std::string& conf_name, Smtp::Server& server) const
    {
      auto iterator = server_confs.find(conf_name);

      if (iterator != server_confs.end())
        iterator->second.ConnectServer(server);
    }

  private:
    class Conf
    {
    public:
      Conf(Data server_data)
      {
        static const std::map<std::string, Smtp::Server::AuthenticationProtocol> auth_protocols = {
          { "plain",      Smtp::Server::PLAIN },
          { "login",      Smtp::Server::LOGIN },
          { "digest_md5", Smtp::Server::DIGEST_MD5 },
          { "md5",        Smtp::Server::MD5 },
          { "cram_md5",   Smtp::Server::CRAM_MD5 }
        };

        hostname           = server_data["hostname"].Value();
        port               = server_data["port"];
        use_tls            = server_data["tls"].Value() == "true";
        use_authentication = server_data["authentication"].NotNil();
        if (use_authentication)
        {
          auto auth_protocol_it = auth_protocols.find(server_data["authentication"]["protocol"].Value());

          if (auth_protocol_it != auth_protocols.end())
          {
            username                = server_data["authentication"]["username"].Value();
            password                = server_data["authentication"]["password"].Value();
            authentication_protocol = auth_protocol_it->second;
          }
        }
      }
      
      void ConnectServer(Smtp::Server& server) const
      {
        if (use_tls)
          server.StartTls();
        if (use_authentication)
          server.Connect(hostname, port, username, password, authentication_protocol);
        else
          server.Connect(hostname, port);
      }
      
    private:
      std::string                          hostname;
      unsigned short                       port;
      bool                                 use_authentication, use_tls;
      Smtp::Server::AuthenticationProtocol authentication_protocol;
      std::string                          username;
      std::string                          password;
    };

    typedef std::map<std::string, Conf> ServerConfs;
    
    MailServers(void)
    {
      DataTree* data_tree = DataTree::Factory::JSON("config/mailers.json");
      
      if (data_tree)
      {
        LoadMailServers(data_tree);
        delete data_tree;
      }
    }
    
    void LoadMailServers(Data data)
    {
      std::for_each(data.begin(), data.end(), [this](Data server_data)
      {
        server_confs.emplace(server_data.Key(), Conf(server_data));
      });
    }

    ServerConfs server_confs;
  };
}

#endif
