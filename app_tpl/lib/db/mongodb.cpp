// compile with: g++ -Wall -g -fPIC -shared lib/db/mongodb.cpp -o lib/db/mongodb.so -I. -std=c++11
#include <crails/databases.hpp>
#include <crails/mongodb.hpp>
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
  return (new MongodbDb(data));
}
