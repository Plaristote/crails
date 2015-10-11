#ifndef  SQL_TABLE_HPP
# define SQL_TABLE_HPP

# include "crails/databases.hpp"
# include <soci/soci.h>
# include <sstream>
# include <iostream>

namespace SQL
{
  class Table
  {
  public:
    Table(soci::session& sql, const std::string& name) : sql(sql), table_name(name)
    {}
    
    template<typename MODEL>
    MODEL          Find(unsigned int id)
    {
      std::cout << "select * from " << table_name << " where id='" << id << '\'' << std::endl;
      soci::rowset<soci::row> rs  = (sql.prepare << "select * from " << table_name << " where id='" << id << '\'');
      auto                    it  = rs.begin();
      auto                    end = rs.end();

      if (it == end)
      {
        std::stringstream stream;
        
        stream << "No document of type " << table_name << " with id " << id;
        throw Crails::Databases::Exception(stream.str());
      }
      {
        soci::row&   row = *it;
        MODEL        model(*this, row);
        
        return (model);
      }
    }
    
    void Query(const std::string& query)
    {
      std::cout << query << std::endl;
      sql << query;
    }
    
    void Query(const std::string& query, int& value)
    {
      std::cout << query << std::endl;
      sql << query, soci::into(value);
    }

    const std::string& GetName(void) const
    {
      return (table_name);
    }

  private:
    soci::session& sql;
    std::string    table_name;
  };
}

#endif