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
#include <Boots/Utils/backtrace.hpp>

void ODB::Database::initialize_for_mysql(Data data)
{
#ifdef ODB_WITH_MYSQL
  db = std::unique_ptr<odb::database>(new odb::mysql::database(
    data["user"].as<std::string>(),
    data["password"].as<std::string>(),
    data["name"].as<std::string>(),
    data["host"].defaults_to<std::string>(""),
    data["port"].defaults_to<unsigned int>(0),
    0, // std::string* socket
    data["charset"].defaults_to<std::string>("")
  ));
#else
  throw boost_ext::runtime_error("ODB was compiled without support for `mysql`");
#endif
}

void ODB::Database::initialize_for_postgresql(Data data)
{
#ifdef ODB_WITH_PGSQL
  db = std::unique_ptr<odb::database>(new odb::pgsql::database(
    data["user"].as<std::string>(),
    data["password"].as<std::string>(),
    data["name"].as<std::string>(),
    data["host"].defaults_to<std::string>(""),
    data["port"].defaults_to<unsigned int>(0),
    data["extra"].defaults_to<std::string>("")
  ));
#else
  throw boost_ext::runtime_error("ODB was compiled without support for `pgsql`");
#endif
}

void ODB::Database::initialize_for_sqlite(Data data)
{
#ifdef ODB_WITH_SQLITE
  db = std::unique_ptr<odb::database>(new odb::sqlite::database(
    data["name"].as<std::string>(),
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
  ));
#else
  throw boost_ext::runtime_error("ODB was compiled without support for `sqlite`");
#endif
}

void ODB::Database::initialize_for_oracle(Data data)
{
#ifdef ODB_WITH_ORACLE
  db = std::unique_ptr<odb::database>(new odb::oracle::database(
    data["user"].as<std::string>(),
    data["password"].as<std::string>(),
    data["name"].as<std::string>(),
    data["host"].defaults_to<std::string>(""),
    data["port"].defaults_to<unsigned int>(0)
  ));
#else
  throw boost_ext::runtime_error("ODB was compiled without support for `oracle`");
#endif
}

using namespace std;
using namespace Crails;
using namespace ODB;

Database::Database(Data settings) : Db(ClassType())
{
  backend = settings["type"].defaults_to<string>("sqlite");

  if (backend == "mysql")
    initialize_for_mysql(settings);
  else if (backend == "pgsql")
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

odb::database& Database::get_database()
{
  connect();
  return *db;
}
