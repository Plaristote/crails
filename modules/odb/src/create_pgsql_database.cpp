#include <crails/odb/database.hpp>
#include <crails/logger.hpp>
#include <boost/lexical_cast.hpp>
#include <list>

using namespace std;
using namespace ODB;
using namespace boost;
using namespace Crails;

#ifdef ODB_WITH_PGSQL

static string pgsql_command_prefix(const Crails::Databases::DatabaseSettings& database_config, std::string user, std::string password)
{
  string command("psql");

  command += " -h " + string(any_cast<const char*>(database_config.at("host")));
  if (database_config.count("port"))
    command += " -p " + lexical_cast<string>(any_cast<unsigned int>(database_config.at("port")));
  command += " -U " + user;
  command += " -c ";
  if (password != "")
    command = "PGPASSWORD=\"" + password + "\" " + command;
  return command;
}

bool pgsql_create_from_settings(const Crails::Databases::DatabaseSettings& database_config, std::string user, std::string password)
{
  string db_user, db_password;
  string db_name = any_cast<const char*>(database_config.at("name"));

  if (database_config.count("user"))
    db_user = any_cast<const char*>(database_config.at("user"));
  if (database_config.count("password"))
    db_password = any_cast<const char*>(database_config.at("password"));
  if (user == "")
  {
    user     = db_user;
    password = db_password;
  }

  {
    string command = pgsql_command_prefix(database_config, user, password);
    list<string> queries;

    if (db_user != user)
      queries.push_back("CREATE USER " + db_user + " WITH PASSWORD '" + db_password + "';");
    queries.push_back("CREATE DATABASE " + db_name + " WITH OWNER=\\\"" + db_user + "\\\";");
    for (string query : queries)
    {
      string full_command(command);

      full_command += '"' + query + '"';
      logger << Logger::Info << ":: running query " << query << Logger::endl;
      logger << Logger::Debug << ":: command " << full_command << Logger::endl;
      std::system(full_command.c_str());
    }
  }
  return true;
}

#endif
