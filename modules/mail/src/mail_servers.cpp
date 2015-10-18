#include <crails/mail_servers.hpp>
#include <algorithm>
#include <map>

using namespace std;
using namespace Crails;

MailServers::MailServers(void)
{
  DataTree* data_tree = DataTree::Factory::JSON("config/mailers.json");
  
  if (data_tree)
  {
    LoadMailServers(data_tree);
    delete data_tree;
  }
}

void MailServers::LoadMailServers(Data data)
{
  for_each(data.begin(), data.end(), [this](Data server_data)
  {
    server_confs.emplace(server_data.Key(), Conf(server_data));
  });
}

void MailServers::ConfigureMailServer(const string& conf_name, Smtp::Server& server) const
{
  auto iterator = server_confs.find(conf_name);

  if (iterator != server_confs.end())
    iterator->second.ConnectServer(server);
}

//
// MailServers::Conf
//
MailServers::Conf::Conf(Data server_data)
{
  static const map<std::string, Smtp::Server::AuthenticationProtocol> auth_protocols = {
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

void MailServers::Conf::ConnectServer(Smtp::Server& server) const
{
  if (use_tls)
    server.StartTls();
  if (use_authentication)
    server.Connect(hostname, port, username, password, authentication_protocol);
  else
    server.Connect(hostname, port);
}