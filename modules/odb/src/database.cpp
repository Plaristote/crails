#include <crails/odb/database.hpp>
#include <crails/odb/database_loaders.hpp>
#include <crails/utils/backtrace.hpp>
#include <crails/logger.hpp>

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
    if (db == NULL)
      throw boost_ext::runtime_error("could not initialize database " + name);
  }
  database_name = Crails::any_cast(settings.at("name"));
}

void Database::connect()
{
}


#ifdef ODB_WITH_PGSQL
bool pgsql_create_from_settings(const Crails::Databases::DatabaseSettings&, std::string, std::string);
bool pgsql_drop_from_settings(const Crails::Databases::DatabaseSettings&, std::string, std::string);
#endif

bool Database::drop_with_settings(const Crails::Databases::DatabaseSettings& settings, std::string user, std::string password)
{
  const string backend_str = defaults_to<const char*>(settings, "type", "sqlite");
  auto         backend_it  = database_types_to_string.find(backend_str);

  if (backend_it == database_types_to_string.end())
    throw UnknownBackend(backend_str);
  else
  {
    switch (backend_it->second)
    {
#ifdef ODB_WITH_PGSQL
    case pgsql:
      return pgsql_drop_from_settings(settings, user, password);
#endif

    default:
      logger << Logger::Error << ":: Database::drop_from_settings not available for backend " << backend_str << Logger::endl;
      break ;
    }
  }
  return false;
  
}

bool Database::create_from_settings(const Crails::Databases::DatabaseSettings& settings, std::string user, std::string password)
{
  const string backend_str = defaults_to<const char*>(settings, "type", "sqlite");
  auto         backend_it  = database_types_to_string.find(backend_str);

  if (backend_it == database_types_to_string.end())
    throw UnknownBackend(backend_str);
  else
  {
    switch (backend_it->second)
    {
#ifdef ODB_WITH_PGSQL
    case pgsql:
      return pgsql_create_from_settings(settings, user, password);
#endif

#ifdef ODB_WITH_SQLITE
    case sqlite:
      logger << Logger::Info << ":: Database::create_from_settings not needed for sqlite backend" << Logger::endl;
      return true;
#endif

    default:
      logger << Logger::Error << ":: Database::create_from_settings not available for backend " << backend_str << Logger::endl;
      break ;
    }
  }
  return false;
}
