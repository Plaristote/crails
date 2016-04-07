#ifndef  ODB_DATABASE_HPP 
# define ODB_DATABASE_HPP

# include <memory>
# include <odb/database.hxx>
# include <crails/databases.hpp>
# include <map>

# ifdef ODB_WITH_MYSQL
#  include <odb/mysql/database.hxx>
# endif

# ifdef ODB_WITH_PGSQL
#  include <odb/pgsql/database.hxx>
# endif

# ifdef ODB_WITH_SQLITE
#  include <odb/sqlite/database.hxx>
# endif

# ifdef ODB_WITH_ORACLE
#  include <odb/oracle/database.hxx>
# endif

# include "migrate.hpp"

namespace ODB
{
  enum DatabaseType
  {
    sqlite,
    pgsql,
    mysql,
    oracle
  };

  class UnknownBackend : public std::exception
  {
    const std::string message;
  public:
    UnknownBackend(const std::string& backend) : message("ODB::Database does not know any '" + backend +"' sql backend") {}

    const char* what() const throw() { return message.c_str(); }
  };

  class Database : public Crails::Databases::Db
  {
  public:
    typedef odb::database* (*Initializer)(const Crails::Databases::DatabaseSettings&);
    typedef std::map<DatabaseType, Initializer> Initializers;

    static const std::string ClassType() { return ("odb"); }

    Database(const Crails::Databases::DatabaseSettings&);

    static bool create_from_settings(const Crails::Databases::DatabaseSettings&, std::string user = "", std::string password = "");
    static bool drop_with_settings(const Crails::Databases::DatabaseSettings&, std::string user = "", std::string password = "");

    odb::database& get_agnostic_database()
    {
      connect();
      return *(db.get());
    }

    template<class T>
    inline T& get_database()
    {
      return reinterpret_cast<T&>(get_agnostic_database());
    }

    const std::string& get_database_name() const { return database_name; }
    DatabaseType       get_type()          const { return backend; }

    void connect();

    inline void migrate(std::function<bool (ODB::Database&, odb::schema_version)> callback = [](ODB::Database&, odb::schema_version) { return true; })
    {
      switch (backend)
      {
#ifdef ODB_WITH_PGSQL
      case pgsql:
        database_migrate<odb::pgsql::database>(*this, get_database<odb::pgsql::database>(), callback);
        break ;
#endif

#ifdef ODB_WITH_MYSQL
      case mysql:
        database_migrate<odb::mysql::database>(*this, get_database<odb::mysql::database>(), callback);
        break ;
#endif

#ifdef ODB_WITH_SQLITE
      case sqlite:
        database_migrate<odb::sqlite::database>(*this, get_database<odb::sqlite::database>(), callback);
        break ;
#endif

#ifdef ODB_WITH_ORACLE
      case oracle:
        database_migrate<odb::oracle::database>(*this, get_database<odb::oracle::database>(), callback);
        break ;
#endif

      default:
        throw std::logic_error("trying to manipulate database types that aren\'t supported by the application");
      }
    }

    inline void drop()
    {
      switch (backend)
      {
#ifdef ODB_WITH_PGSQL
      case pgsql:
        database_drop<odb::pgsql::database>(get_database<odb::pgsql::database>());
        break ;
#endif

#ifdef ODB_WITH_MYSQL
      case mysql:
        database_drop<odb::mysql::database>(get_database<odb::mysql::database>());
        break ;
#endif

#ifdef ODB_WITH_SQLITE
      case sqlite:
        database_drop<odb::sqlite::database>(get_database<odb::sqlite::database>());
        break ;
#endif

#ifdef ODB_WITH_ORACLE
      case oracle:
        database_drop<odb::oracle::database>(get_database<odb::oracle::database>());
        break ;
#endif

      default:
        throw std::logic_error("trying to drop database types that aren\'t supported by the application");
      }
    }

  private:
    std::unique_ptr<odb::database> db;
    DatabaseType                   backend;
    std::string                    database_name;
    static const Initializers      initializers;
  };
}

#endif
