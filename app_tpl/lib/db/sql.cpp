#include <crails/databases.hpp>
#include <crails/sql/database.hpp>
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

SQL::Database* create_mysql_database(Data data)
{
  std::string mbf = "mysql";
  std::string cmd = "dbname=" + data["database"].Value();
    
  if (data["user"].NotNil())
    cmd += " user=" + data["user"].Value();
  if (data["password"].NotNil())
    cmd += " password=" + data["password"].Value();
  if (data["host"].NotNil())
    cmd += " host=" + data["host"].Value();
  if (data["port"].NotNil())
    cmd += " port=" + data["port"].Value();
  if (data["unix_socket"].NotNil())
    cmd += " unix_socket=" + data["unix_socket"].Value();
  return (new SQL::Database(mbf, cmd));
}

SQL::Database* create_sqlite3_database(Data data)
{
  std::string mbf = "sqlite3";
  std::string cmd = "sqlite.db";

  if (data["file"].NotNil())
    cmd = data["file"].Value();
  return (new SQL::Database(mbf, cmd));
}

SQL::Database* create_postgresql_database(Data data)
{
  std::string mbf = "postgresql";
  std::string cmd = "dbname=" + data["database"].Value();

  return (new SQL::Database(mbf, cmd));
}

Databases::Db* hook_callback(Data data)
{
  std::string    backend = data["type"].Value();
  SQL::Database* (*create_function)(Data data);
  SQL::Database* database;

  if (backend == "mysql")
    create_function = create_mysql_database;
  else if (backend == "postgresql")
    create_function = create_postgresql_database;
  else
    create_function = create_sqlite3_database;
  database = (*create_function)(data);
  database->SetDbName(data["database"].Value());
  return (database);
}

