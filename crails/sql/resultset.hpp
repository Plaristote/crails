#ifndef  SQL_RESULTSET_HPP
# define SQL_RESULTSET_HPP

# include "table.hpp"
# include "include/sql.hpp"

namespace SQL
{
  template<typename MODEL>
  class ResultSet
  {
  public:
    ResultSet(Table& table, soci::rowset<soci::row> rs) : table(table), rs(rs)
    {
      iterator = rs.begin();
    }
    
    void Each(std::function<bool (MODEL&)> callback)
    {
      typename std::list<MODEL>::iterator it  = list.begin();
      typename std::list<MODEL>::iterator end = list.end();
      
      for (; it != end ; ++it)
      {
        if (!(callback(*it)))
          return ;
      }
      while (iterator != rs.end())
      {
        soci::row& row = *iterator;
        MODEL      model(table, row);

        list.push_back(model);
        if (!(callback(*(list.rbegin()))))
          return ;
        ++iterator;
      }
    }
    
    static ResultSet* Query(const std::string& query)
    {
      const std::string database        = MODEL::DatabaseName();
      const std::string collection_name = MODEL::CollectionName();
      SqlDb&            sqldb           = Databases::singleton::Get()->GetDb<SqlDb>(database);
      Table             collection      = sqldb[collection_name];
      soci::rowset<soci::row> res = (sqldb.sql.prepare << query);

      return (new ResultSet<MODEL>(collection, res));
    }
    
    std::list<MODEL>& Entries(void)
    {
      Each([](MODEL&) -> bool { return (true); });
      return (list);
    }

  private:
    Table                             table;
    soci::rowset<soci::row>           rs;
    soci::rowset<soci::row>::iterator iterator;
    std::list<MODEL>                  list;
  };  
}

#endif