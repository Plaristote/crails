#include <iostream>
#include <crails/server.hpp>
#include <crails/databases.hpp>
#include <crails/mail_servers.hpp>

using namespace std;

int main(int argc, char **argv)
{
  MailServers::singleton::Initialize();
  Databases::Factories::singleton::Initialize();
  Databases::Factories::singleton::Get()->Load("../lib/db");
  CrailsServer::Launch(argc, argv);
  Rackables<Databases::DbFactory>::singleton::Finalize();
  MailServers::singleton::Finalize();
  return (0);
}
