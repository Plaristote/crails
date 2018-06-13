#ifndef  DB_TRANSACTION_HPP
# define DB_TRANSACTION_HPP

# include <string>
# include <memory>

namespace odb
{
  class transaction;
  class database;
}

namespace Db
{
  struct Transaction
  {
    Transaction();
    ~Transaction();

    std::string get_database_name() const { return database_name; }
    odb::database& get_database();

    void require(const std::string& name);
    void start(const std::string& name, odb::database&);
    void commit();
    void rollback();

  private:
    void cleanup();

    std::string                       database_name;
    std::unique_ptr<odb::transaction> odb_transaction;
    odb::database*                    odb_database;
  };
}

#endif
