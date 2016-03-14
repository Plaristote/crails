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

    DatabaseType get_type() const { return backend; }

    void connect(void);
  private:

    std::unique_ptr<odb::database> db;
    DatabaseType                   backend;
    static const Initializers      initializers;
  };
}

#endif
