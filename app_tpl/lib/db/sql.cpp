// compile with: g++ -Wall -g -fPIC -shared lib/db/sql.cpp -o lib/db/sql.so -I. -std=c++11
#include <crails/databases.hpp>
#include <crails/sql.hpp>
#include <string>

extern "C"
{
  std::string    hook_name(void);
  Databases::Db* hook_callback(Data);
}

std::string    hook_name(void)
{
  return ("sql");
}

Databases::Db* hook_callback(Data data)
{
  return (new SqlDb(data));
}
