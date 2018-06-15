#ifndef  DB_TRANSACTION_HPP
# define DB_TRANSACTION_HPP

# include <string>
# include <memory>

namespace odb
{
  class transaction;
  class session;
  class database;
}

namespace ODB
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

    static const bool use_session;

    std::string                       database_name;
    std::unique_ptr<odb::transaction> odb_transaction;
    std::unique_ptr<odb::session>     odb_session;
    odb::database*                    odb_database;
  };
}

#endif
