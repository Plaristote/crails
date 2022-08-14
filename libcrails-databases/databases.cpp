#include "databases.hpp"

using namespace std;
using namespace Crails;

thread_local Crails::Databases Crails::databases;

Databases::Database::~Database()
{
}

void Databases::cleanup_databases()
{
  for (auto it = databases.begin() ; it != databases.end() ; ++it)
    delete *it;
  databases.clear();
}

Databases::Database* Databases::get_database_from_name(const std::string& key)
{
  for (auto it = databases.begin() ; it != databases.end() ; ++it)
  {
    if (**it == key)
      return (*it);
  }
  return (0);
}
