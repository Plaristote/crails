#include <crails/mail_servers.hpp>
#include <algorithm>
#include <map>

using namespace std;
using namespace Crails;

MailServers::MailServers(void)
{
  data_tree.from_json_file("config/mailers.json");
  load_mail_servers(data_tree.as_data());
}

void MailServers::load_mail_servers(Data data)
{
  data.each([this](Data server_data)
  {
    server_confs.emplace(server_data.get_key(), Conf(server_data));
  });
}

void MailServers::configure_mail_server(const string& conf_name, Smtp::Server& server) const
{
  auto iterator = server_confs.find(conf_name);

  if (iterator != server_confs.end())
    iterator->second.connect_server(server);
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

  hostname           = server_data["hostname"].as<string>();
  port               = server_data["port"];
  use_tls            = server_data["tls"].as<string>() == "true";
  use_authentication = server_data["authentication"]["username"].defaults_to<string>("") != "";
  if (use_authentication)
  {
    auto auth_protocol_it = auth_protocols.find(server_data["authentication"]["protocol"].as<string>());

    if (auth_protocol_it != auth_protocols.end())
    {
      username                = server_data["authentication"]["username"].as<string>();
      password                = server_data["authentication"]["password"].as<string>();
      authentication_protocol = auth_protocol_it->second;
    }
  }
}

void MailServers::Conf::connect_server(Smtp::Server& server) const
{
  if (use_tls)
    server.start_tls();
  if (use_authentication)
    server.connect(hostname, port, username, password, authentication_protocol);
  else
    server.connect(hostname, port);
}
