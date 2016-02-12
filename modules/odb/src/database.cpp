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

template<typename V>
static V defaults_to(const std::map<std::string, boost::any>& a, const std::string& k, const V def)
{
  typename std::map<std::string, boost::any>::const_iterator it = a.find(k);

  if (it == a.end())
    return def;
  return boost::any_cast<V>(it->second);
}

void ODB::Database::initialize_for_mysql(const Databases::DatabaseSettings& settings)
{
#ifdef ODB_WITH_MYSQL
  db = std::unique_ptr<odb::database>(new odb::mysql::database(
    boost::any_cast<std::string>(settings.at("user")),
    boost::any_cast<std::string>(settings.at("password")),
    boost::any_cast<std::string>(settings.at("name")),
    defaults_to<std::string> (settings, "host", ""),
    defaults_to<unsigned int>(settings, "port", 0)
    0,
    defaults_to<std::string>(settings, "charset", "")
  ));
#else
  throw boost_ext::runtime_error("ODB was compiled without support for `mysql`");
#endif
}

void ODB::Database::initialize_for_postgresql(const Databases::DatabaseSettings& settings)
{
#ifdef ODB_WITH_PGSQL
  db = std::unique_ptr<odb::database>(new odb::pgsql::database(
    boost::any_cast<std::string>(settings.at("user")),
    boost::any_cast<std::string>(settings.at("password")),
    boost::any_cast<std::string>(settings.at("name")),
    defaults_to<std::string> (settings, "host",  ""),
    defaults_to<unsigned int>(settings, "port",  0),
    defaults_to<std::string> (settings, "extra", "")
  ));
#else
  throw boost_ext::runtime_error("ODB was compiled without support for `pgsql`");
#endif
}

void ODB::Database::initialize_for_sqlite(const Databases::DatabaseSettings& settings)
{
#ifdef ODB_WITH_SQLITE
  db = std::unique_ptr<odb::database>(new odb::sqlite::database(
    boost::any_cast<std::string>(settings.at("name")),
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
    boost::any_cast<std::string>(settings.at("user")),
    boost::any_cast<std::string>(settings.at("password")),
    boost::any_cast<std::string>(settings.at("name")),
    defaults_to<std::string> (settings, "host", ""),
    defaults_to<unsigned int>(settings, "port", 0)
  ));
#else
  throw boost_ext::runtime_error("ODB was compiled without support for `oracle`");
#endif
}

Database::Database(const Databases::DatabaseSettings& settings) : Db(ClassType())
{
  backend = defaults_to<std::string>(settings, "type", "sqlite");

  if (backend == "mysql")
    initialize_for_mysql(settings);
  else if (backend == "pgsql" || backend == "postgres")
    initialize_for_postgresql(settings);
  else if (backend == "oracle")
    initialize_for_oracle(settings);
  else
    initialize_for_sqlite(settings);
}

void Database::connect()
{
  if (db == NULL)
    throw boost_ext::runtime_error("Could not create a database object");
}
