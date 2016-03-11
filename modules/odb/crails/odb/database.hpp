#ifndef  ODB_DATABASE_HPP 
# define ODB_DATABASE_HPP

# include <memory>
# include <odb/database.hxx>
# include <crails/databases.hpp>
# include <map>

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
    typedef void (Database::*Initializer)(const Crails::Databases::DatabaseSettings&);
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
    void initialize_for_mysql(const Crails::Databases::DatabaseSettings&);
    void initialize_for_postgresql(const Crails::Databases::DatabaseSettings&);
    void initialize_for_sqlite(const Crails::Databases::DatabaseSettings&);
    void initialize_for_oracle(const Crails::Databases::DatabaseSettings&);

    std::unique_ptr<odb::database> db;
    DatabaseType                   backend;
    static const Initializers      initializers;
  };
}

#endif
