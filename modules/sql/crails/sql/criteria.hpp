#ifndef  SQL_CRITERIA_HPP
# define SQL_CRITERIA_HPP

# include <soci/soci.h>
# include <crails/sql/table.hpp>
# include <crails/sql/database.hpp>
# include <Boots/Utils/smart_pointer.hpp>

namespace SQL
{
  class IField;

  class Criteria
  {
  public:
    typedef std::pair<std::string, char> OrderItem;
    typedef std::list<OrderItem>         OrderItems;

    enum SortOrder
    {
      Asc,
      Desc
    };

    enum Operator
    {
      GreaterThan,
      GreaterThanOrEqualTo,
      LessThan,
      LessThanOrEqualTo,
      EqualTo,
      DifferentFrom,
      Like
    };

    Criteria(Table table);

    unsigned int count(void);
    void         remove(void);
    void         remove_one(void);

    Criteria& skip(int n_to_skip);
    Criteria& limit(int n_to_return);
    Criteria& order_by(const std::string& key, SortOrder sort_order);
    Criteria& or_();

    template<typename T>
    Criteria& where(const std::string& key, Operator op, const T value)
    {
      std::stringstream value_stream;
      std::string       value_as_string;

      value_stream << value;
      value_as_string = value_stream.str();
      where_string.reserve(where_string.length() + key.length() + value_as_string.length() + 10);
      where_string += (where_string.length() == 0) ? " WHERE " : " AND ";
      where_string += key + ' ' + operator_to_string(op) + ' ' + value_as_string;
      return *this;
    }

    template<typename MODEL>
    void each(std::function<bool (MODEL)> functor)
    {
      if (results.Null())
        fetch_results();
      for (auto it = results->begin() ; it != results->end() ; ++it)
      {
        MODEL model(table, *it);

        if (!(functor(model)))
          break ;
      }
    }

    void update(const std::vector<IField*>& fields) const;
    long insert(const std::vector<IField*>& fields) const;

    template<typename MODEL>
    static Criteria prepare()
    {
      const std::string database        = MODEL::DatabaseName();
      const std::string collection_name = MODEL::CollectionName();
      SQL::Database&    sqldb           = CRAILS_DATABASE(SQL,database);
      Table             collection      = sqldb[collection_name];

      return (Criteria(collection));
    }

  private:
    void        fetch_results();
    std::string append_order_by_to_query(std::string& query) const;
    const char* operator_to_string(Operator op) const;

    Table                       table;
    int                         n_to_skip, n_to_return;
    OrderItems                  order_items;
    std::string                 where_string;
    SP(soci::rowset<soci::row>) results;
  };
}

#endif