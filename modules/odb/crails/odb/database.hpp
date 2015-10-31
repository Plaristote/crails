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

    Database(Data settings);

    const std::string& get_backend_name() const { return backend; }

    void connect(void);
  private:
    void initialize_for_mysql(Data);
    void initialize_for_postgresql(Data);
    void initialize_for_sqlite(Data);
    void initialize_for_oracle(Data);

    std::unique_ptr<odb::database> db;
    std::string                    backend;
  };
}

#endif
