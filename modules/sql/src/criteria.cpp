#include <crails/sql/criteria.hpp>
#include <crails/sql/model.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace SQL;
using namespace soci;

Criteria::Criteria(Table table) : table(table), n_to_skip(0), n_to_return(0)
{
}

unsigned int Criteria::count()
{
  unsigned int count;
  string       query = "SELECT COUNT(*) FROM " + table.get_name() + where_string;

  table.sql << query, into(count);
  return count;
}

void Criteria::remove()
{
  table.sql << ("DELETE * FROM " + table.get_name() + where_string);
}

void Criteria::remove_one()
{
  table.sql << ("DELETE * FROM " + table.get_name() + where_string + " LIMIT 1");
}

void Criteria::update(const vector<IField*>& fields) const
{
  std::string query;
  bool        need_coma = false;
  
  query.reserve(12 + table.get_name().length() + where_string.length() + fields.size() * 15);
  query = "UPDATE " + table.get_name() + " SET ";
  for (auto it = fields.begin() ; it != fields.end() ; ++it)
  {
    if (need_coma)
      query += ',';
    if ((*it)->has_changed)
    {
      query += (*it)->key + '=' + (*it)->get_string();
      need_coma = true;
    }
  }
  query += where_string;
  table.sql << query;
}

long Criteria::insert(const vector<IField*>& fields) const
{
  long        id;
  std::string query;
  bool        need_coma = false;

  query.reserve(21 + table.get_name().length() + fields.size() * 10);
  query = "INSERT INTO " + table.get_name() + " VALUES(";
  for (auto it = fields.begin() ; it != fields.end() ; ++it)
  {
    if (need_coma)
      query += ',';
    else
      need_coma = true;
    query += (*it)->get_string();
  }
  query += ')';
  table.sql << query;
  table.sql.get_last_insert_id(table.get_name(), id);
  return id;
}

void Criteria::fetch_results()
{
  std::string query;

  query.reserve(30 + table.get_name().length() + where_string.length() + order_items.size() * 10);
  query = "SELECT * FROM " + table.get_name() + where_string;
  if (n_to_skip >= 0 && n_to_return > 0)
    query += " LIMIT " + boost::lexical_cast<string>(n_to_skip) + ',' + boost::lexical_cast<string>(n_to_return);
  if (order_items.size() > 0)
    query = append_order_by_to_query(query);
  results = new soci::rowset<soci::row>((table.sql.prepare << query));
}

std::string Criteria::append_order_by_to_query(std::string& query) const
{
  bool need_coma = false;

  query += " ORDER BY ";
  for (auto it = order_items.begin() ; it != order_items.end() ; ++it)
  {
    if (need_coma)
      query += ',';
    else
      need_coma = true;
    query += (*it).first;
    switch ((*it).second)
    {
      case Asc:  query += " ASC";
      case Desc: query += " DESC";
    }
  }
  return query;
}

Criteria& Criteria::skip(int n_to_skip)
{
  this->n_to_skip = n_to_skip;
  return *this;
}

Criteria& Criteria::limit(int n_to_return)
{
  this->n_to_return = n_to_return;
  return *this;
}

Criteria& Criteria::order_by(const std::string& key, SortOrder sort_order)
{
  order_items.push_back(OrderItem(key, sort_order));
  return *this;
}

Criteria& Criteria::or_()
{
  where_string.reserve(where_string.length() + 6);
  where_string.insert(0, "(");
  where_string += ") OR ";
  return *this;
}

const char* Criteria::operator_to_string(Operator op) const
{
  switch (op)
  {
    case GreaterThan:          return ">";
    case GreaterThanOrEqualTo: return ">=";
    case LessThan:             return "<";
    case LessThanOrEqualTo:    return "<=";
    case EqualTo:              return "=";
    case DifferentFrom:        return "!=";
    case Like:                 return "LIKE";
  }
  return "=";
}

namespace SQL
{
  template<>
  Criteria& Criteria::where<std::string>(const std::string& key, Operator op, const std::string value)
  {
    const unsigned char estimated_value_length = value.length() * 1.1 + 4;

    where_string.reserve(where_string.length() + key.length() + estimated_value_length + 12);
    where_string += (where_string.length() == 0) ? " WHERE " : " AND ";
    where_string += key + ' ' + operator_to_string(op) + " \"";
    for (std::string::size_type i = 0; i < value.length(); ++i)
    {
      switch (value[i])
      {
      case '"':
      case '\\':
        where_string += '\\';
      default:
        where_string += value[i];
      }
    }
    where_string += '"';
    return *this;
  }
}