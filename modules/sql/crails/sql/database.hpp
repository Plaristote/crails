#ifndef  SQL_DATABASE_HPP
# define SQL_DATABASE_HPP

# include <soci/soci.h>
# include <string>
# include <crails/databases.hpp>

namespace SQL
{
  class TableDescription
  {
  public:
    struct Field
    {
      Field(const std::string& name, const std::string& type) : name(name), type(type)
      {}
      
      Field(void)
      {}

      std::string name;
      std::string type;
    };
    
    TableDescription(soci::session& sql, const std::string name);
    
    void SetTableSchema(std::vector<Field> updated_fields);
    void InsertTable(std::vector<Field> updated_fields);
    void UpdateTable(std::vector<Field> updated_fields);
    
  private:
    std::vector<Field> fields;
    std::string        table_name;
    soci::session&     sql;
    bool               table_exists;
  };
  
  class Table;

  class Database : public Crails::Databases::Db
  {
  public:
    static const std::string ClassType(void) { return ("sql"); }

    Database(Data settings);

    const std::string& get_backend_name() const { return factory; }

    void             SetDbName(const std::string& name)
    {
      this->name = name;
    }

    void             connect(void);

    Table            operator[](const std::string& name);
    TableDescription Describe(const std::string& table_name);
    bool             TableExists(const std::string& table_name);

    soci::session sql;
  protected:
    std::string   name;
    std::string   factory, connect_cmd;
  private:
    void InitializeForMySQL(Data);
    void InitializeForPostgreSQL(Data);
    void InitializeForSqlite(Data);

    bool          connected;
  };
}

#include "table.hpp"

#endif
