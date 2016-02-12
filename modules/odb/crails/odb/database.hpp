#ifndef  ODB_DATABASE_HPP 
# define ODB_DATABASE_HPP

# include <memory>
# include <odb/database.hxx>
# include <crails/databases.hpp>

namespace ODB
{
  class Database : public Crails::Databases::Db
  {
  public:
    static const std::string ClassType() { return ("odb"); }

    Database(const Crails::Databases::DatabaseSettings&);

    const std::string& get_backend_name() const { return backend; }

    template<class T>
    T& get_database()
    {
      connect();
      return *(reinterpret_cast<T*>(db.get()));
    }

    void connect(void);
  private:
    void initialize_for_mysql(const Crails::Databases::DatabaseSettings&);
    void initialize_for_postgresql(const Crails::Databases::DatabaseSettings&);
    void initialize_for_sqlite(const Crails::Databases::DatabaseSettings&);
    void initialize_for_oracle(const Crails::Databases::DatabaseSettings&);

    std::unique_ptr<odb::database> db;
    std::string                    backend;
  };
}

#endif
