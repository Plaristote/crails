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
    soci::session& sql;

    Table(soci::session& sql, const std::string& name) : sql(sql), table_name(name)
    {}

    template<typename MODEL>
    MODEL          find(unsigned int id)
    {
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

    const std::string& get_name(void) const
    {
      return (table_name);
    }

  private:
    std::string    table_name;
  };
}

#endif