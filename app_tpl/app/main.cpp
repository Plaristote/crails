#include <iostream>
#include <crails/server.hpp>
#include <crails/databases.hpp>

using namespace std;

int main(int argc, char **argv)
{
  Databases::Factories::singleton::Initialize();
  Databases::Factories::singleton::Get()->Load("../lib/db");
  CrailsServer::Launch(argc, argv);
  Rackables<Databases::DbFactory>::singleton::Finalize();
  return (0);
}
