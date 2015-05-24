#include "crails/databases.hpp"

using namespace std;
using namespace Crails;

Databases::Db::~Db()
{
}

void Databases::cleanup_databases()
{
  for (auto it = databases.begin() ; it != databases.end() ; ++it)
    delete *it;
  databases.clear();
}

Databases::Db* Databases::get_database_from_name(const std::string& key)
{
  for (auto it = databases.begin() ; it != databases.end() ; ++it)
  {
    if (**it == key)
      return (*it);
  }
  return (0);
}
