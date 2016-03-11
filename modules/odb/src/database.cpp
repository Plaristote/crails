#include <crails/odb/database.hpp>
#ifdef ODB_WITH_MYSQL
# include <odb/mysql/database.hxx>
#endif
#ifdef ODB_WITH_PGSQL
# include <odb/pgsql/database.hxx>
#endif
#ifdef ODB_WITH_SQLITE
# include <odb/sqlite/database.hxx>
#endif
#ifdef ODB_WITH_ORACLE
# include <odb/oracle/database.hxx>
#endif
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

const Database::Initializers Database::initializers = {
  { sqlite, &Database::initialize_for_sqlite },
  { pgsql,  &Database::initialize_for_postgresql },
  { mysql,  &Database::initialize_for_mysql },
  { oracle, &Database::initialize_for_oracle }
};

template<typename V>
static V defaults_to(const std::map<std::string, boost::any>& a, const std::string& k, const V def)
{
  typename std::map<std::string, boost::any>::const_iterator it = a.find(k);

  if (it == a.end())
    return def;
  return boost::any_cast<V>(it->second);
}

Database::Database(const Databases::DatabaseSettings& settings) : Db(ClassType())
{
  const string backend_str = defaults_to<const char*>(settings, "type", "sqlite");
  auto         backend_it  = database_types_to_string.find(backend_str);

  if (backend_it == database_types_to_string.end())
    throw UnknownBackend(backend_str);
  else
  {
    backend = backend_it->second;
    auto initializer = initializers.at(backend);
    (this->*initializer)(settings);
  }
}

void ODB::Database::initialize_for_mysql(const Databases::DatabaseSettings& settings)
{
#ifdef ODB_WITH_MYSQL
  db = std::unique_ptr<odb::database>(new odb::mysql::database(
    boost::any_cast<cosnt char*>(settings.at("user")),
    boost::any_cast<const char*>(settings.at("password")),
    boost::any_cast<const char*>(settings.at("name")),
    defaults_to<const char*> (settings, "host", ""),
    defaults_to<unsigned int>(settings, "port", 0)
    0,
    defaults_to<const char*>(settings, "charset", "")
  ));
#else
  throw boost_ext::runtime_error("ODB was compiled without support for `mysql`");
#endif
}

void ODB::Database::initialize_for_postgresql(const Databases::DatabaseSettings& settings)
{
#ifdef ODB_WITH_PGSQL
  db = std::unique_ptr<odb::database>(new odb::pgsql::database(
    boost::any_cast<const char*>(settings.at("user")),
    boost::any_cast<const char*>(settings.at("password")),
    boost::any_cast<const char*>(settings.at("name")),
    defaults_to<const char*> (settings, "host",  ""),
    defaults_to<unsigned int>(settings, "port",  0),
    defaults_to<const char*> (settings, "extra", "")
  ));
#else
  throw boost_ext::runtime_error("ODB was compiled without support for `pgsql`");
#endif
}

void ODB::Database::initialize_for_sqlite(const Databases::DatabaseSettings& settings)
{
#ifdef ODB_WITH_SQLITE
  db = std::unique_ptr<odb::database>(new odb::sqlite::database(
    boost::any_cast<const char*>(settings.at("name")),
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
  ));
#else
  throw boost_ext::runtime_error("ODB was compiled without support for `sqlite`");
#endif
}

void ODB::Database::initialize_for_oracle(const Databases::DatabaseSettings& settings)
{
#ifdef ODB_WITH_ORACLE
  db = std::unique_ptr<odb::database>(new odb::oracle::database(
    boost::any_cast<const char*>(settings.at("user")),
    boost::any_cast<const char*>(settings.at("password")),
    boost::any_cast<const char*>(settings.at("name")),
    defaults_to<const char*> (settings, "host", ""),
    defaults_to<unsigned int>(settings, "port", 0)
  ));
#else
  throw boost_ext::runtime_error("ODB was compiled without support for `oracle`");
#endif
}

void Database::connect()
{
  if (db == NULL)
    throw boost_ext::runtime_error("Could not create a database object");
}
