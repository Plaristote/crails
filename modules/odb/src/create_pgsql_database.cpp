#include <crails/odb/database.hpp>
#include <crails/logger.hpp>
#include <crails/any_cast.hpp>
#include <boost/lexical_cast.hpp>
#include <list>
#include <sys/wait.h>

using namespace std;
using namespace ODB;
using namespace boost;
using namespace Crails;

#ifdef ODB_WITH_PGSQL

static string pgsql_command_prefix(const Crails::Databases::DatabaseSettings& database_config, std::string user, std::string password)
{
  string command("psql");

  command += " -h " + string(Crails::any_cast(database_config.at("host")));
  if (database_config.count("port"))
    command += " -p " + lexical_cast<string>(any_cast<unsigned int>(database_config.at("port")));
  command += " -U " + user;
  command += " -c ";
  if (password != "")
    command = "PGPASSWORD=\"" + password + "\" " + command;
  return command;
}

static void initialize_credentials(const Crails::Databases::DatabaseSettings& database_config, std::string& db_user, std::string& db_password, std::string& user, std::string& password)
{
  if (database_config.count("user"))
    db_user = Crails::any_cast(database_config.at("user"));
  if (database_config.count("password"))
    db_password = Crails::any_cast(database_config.at("password"));
  if (user == "")
  {
    user     = db_user;
    password = db_password;
  }
}

static bool pgsql_run_queries(const Crails::Databases::DatabaseSettings& database_config, std::string user, std::string password, std::list<std::string> queries, bool ignore_failure = false)
{
  string db_user, db_password;
  string db_name = Crails::any_cast(database_config.at("name"));
  string command;

  initialize_credentials(database_config, db_user, db_password, user, password);
  command = pgsql_command_prefix(database_config, user, password);
  for (string query : queries)
  {
    string full_command(command);
    int    status;

    full_command += '"' + query + '"';
    logger << Logger::Info << ":: running query " << query << Logger::endl;
    logger << Logger::Debug << ":: command " << full_command << Logger::endl;
    status = std::system(full_command.c_str());
    if (ignore_failure == false)
    {
      if (status < 0)
      {
        logger << Logger::Debug << ":: failed to run command" << Logger::endl;
        return false;
      }
      else
      {
        if (WIFEXITED(status))
        {
          if (WEXITSTATUS(status) != 0)
          {
            logger << Logger::Debug << ":: command returned with exit status " << WEXITSTATUS(status) << Logger::endl;
            return false;
          }
        }
        else
        {
          logger << Logger::Debug << ":: command didn't return" << Logger::endl;
         return false;
        }
      }
    }
  }
  return true;
}

bool pgsql_create_from_settings(const Crails::Databases::DatabaseSettings& database_config, std::string user, std::string password)
{
  string db_user, db_password;
  string db_name = Crails::any_cast(database_config.at("name"));
  list<string> queries;

  initialize_credentials(database_config, db_user, db_password, user, password);
  if (db_user != user)
  {
    queries.push_back("CREATE USER " + db_user + " WITH PASSWORD '" + db_password + "';");
    pgsql_run_queries(database_config, user, password, queries, true);
    queries.clear();
  }
  queries.push_back("CREATE DATABASE " + db_name + " WITH OWNER=\\\"" + db_user + "\\\";");
  return pgsql_run_queries(database_config, user, password, queries);
}

bool pgsql_drop_from_settings(const Crails::Databases::DatabaseSettings& database_config, std::string user, std::string password)
{
  string       db_name = Crails::any_cast(database_config.at("name"));
  list<string> queries;

  queries.push_back("update pg_database set datallowconn = 'false' where datname = '" + db_name + "';");
  queries.push_back("SELECT pg_terminate_backend(pid) FROM pg_stat_activity WHERE datname = '" + db_name + "';");
  queries.push_back("DROP DATABASE " + db_name);
  return pgsql_run_queries(database_config, user, password, queries);
}

#endif
