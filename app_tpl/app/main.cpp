#include <iostream>
#include <crails/server.hpp>
#include <crails/databases.hpp>
#include <crails/mail_servers.hpp>

using namespace std;
using namespace Crails;

int main(int argc, char **argv)
{
  MailServers::singleton::Initialize();
  Server::Launch(argc, argv);
  MailServers::singleton::Finalize();
  return (0);
}
