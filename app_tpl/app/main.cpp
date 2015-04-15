#include <iostream>
#include <crails/server.hpp>
#include <crails/databases.hpp>
#include <crails/mail_servers.hpp>

using namespace std;

int main(int argc, char **argv)
{
  MailServers::singleton::Initialize();
  CrailsServer::Launch(argc, argv);
  MailServers::singleton::Finalize();
  return (0);
}
