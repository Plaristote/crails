#include <crails/odb/database.hpp>

void Database::initialize_for_mysql(Data data)
{
  db = new odb::mysql::database(
    data["user"].as<std::string>(),
    data["password"].as<std::string>(),
    data["name"].as<std::string>(),
    data["host"].defaults_to<std::string>(""),
    data["port"].defaults_to<unsigned int>(0),
    0, // std::string* socket
    data["charset"].defaults_to<std::string>("")
  );
}

void Database::initialize_for_postgresql(Data data)
{
  db = new odb::pgsql::database(
    data["user"].as<std::string>(),
    data["password"].as<std::string>(),
    data["name"].as<std::string>(),
    data["host"].defaults_to<std::string>(""),
    data["port"].defaults_to<unsigned int>(0),
    data["extra"].defaults_to<std::string>("")
  );
}

void Database::initialize_for_sqlite(Data data)
{
  db = new odb::sqlite::database(
    data["name"].as<std::string>(),
    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
  );
}

void Database::initialize_for_oracle(Data data)
{
  db = new odb::oracle::database(
    data["user"].as<std::string>(),
    data["password"].as<std::string>(),
    data["name"].as<std::string>(),
    data["host"].defaults_to<std::string>(""),
    data["port"].defaults_to<unsigned int>(0)
  );
}

using namespace std;
using namespace Crails;

Database::Database(Data settings) : Db(ClassType()), connected(false)
{
  backend = settings["type"].defaults_to<string>("sqlite");

  if (backend == "mysql")
    initialize_for_mysql(settings);
  else if (backend == "postgresql")
    initialize_for_postgresql(settings);
  else if (backend == "oracle")
    initialize_for_oracle(settings);
  else
    initialize_for_sqlite(settings);
}

void Database::connect()
{
}
