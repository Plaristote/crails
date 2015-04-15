#include "crails/databases.hpp"

using namespace std;

void Databases::CleanupDatabases()
{
  for (auto it = databases.begin() ; it != databases.end() ; ++it)
    delete *it;
  databases.clear();
}

Databases::Db* Databases::GetDatabaseFromName(const std::string& key)
{
  for (auto it = databases.begin() ; it != databases.end() ; ++it)
  {
    if (**it == key)
      return (*it);
  }
  return (0);
}
