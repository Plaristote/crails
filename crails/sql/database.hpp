#ifndef  SQL_DATABASE_HPP
# define SQL_DATABASE_HPP

# include <soci/soci.h>
# include <soci/soci-mysql.h>
# include <string>

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

  class Database
  {
  public:
    Database(soci::mysql_backend_factory mbf, const std::string& cmd) : mbf(mbf), connect_cmd(cmd)
    {
    }
    
    void             SetDbName(const std::string& name)
    {
      this->name = name;
    }
    
    void             OpenConnection(void)
    {
      sql.open(mbf, connect_cmd);
    }
    
    Table            operator[](const std::string& name);
    TableDescription Describe(const std::string& table_name);
    bool             TableExists(const std::string& table_name);

    soci::session sql;
  protected:
    std::string   name;
    soci::mysql_backend_factory mbf;
    std::string                 connect_cmd;
  };
}

#include "table.hpp"

#endif