// compile with: g++ -Wall -g -fPIC -shared lib/db/mongodb.cpp -o lib/db/mongodb.so -I. -std=c++11
#include <crails/databases.hpp>
#include <crails/mongodb/database.hpp>
#include <string>

extern "C"
{
  std::string    hook_name(void);
  Databases::Db* hook_callback(Data);
}

std::string    hook_name(void)
{
  return ("mongodb");
}

Databases::Db* hook_callback(Data data)
{
  MongoDB::Database* database = 0;
  std::string        name     = data["database"].Value();
  std::string        host     = data["host"].Value();
  unsigned short     port     = data["port"].Nil() ? (unsigned short)27017 : (unsigned short)data["port"];

  database = new MongoDB::Database(name, host, port);
  if (data["username"].NotNil())
    database->AuthenticateWith(data["username"].Value(), data["password"].Value());
  return (database);
}
