#include <crails/odb/database.hpp>
#include <crails/odb/database_loaders.hpp>
#include <crails/utils/backtrace.hpp>

using namespace std;
using namespace ODB;
using namespace Crails;

std::map<std::string, DatabaseType> database_types_to_string = {
  { "sqlite", sqlite },
  { "pgsql",  pgsql },
  { "mysql",  mysql },
  { "oracle", oracle }
};

Database::Database(const Databases::DatabaseSettings& settings) : Db(ClassType())
{
  const string backend_str = defaults_to<const char*>(settings, "type", "sqlite");
  auto         backend_it  = database_types_to_string.find(backend_str);

  if (backend_it == database_types_to_string.end())
    throw UnknownBackend(backend_str);
  else
  {
    backend = backend_it->second;
    db      = std::unique_ptr<odb::database>(initializers.at(backend)(settings));
  }
}

void Database::connect()
{
  if (db == NULL)
    throw boost_ext::runtime_error("Could not create a database object");
}
