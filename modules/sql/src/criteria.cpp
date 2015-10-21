#include <crails/sql/criteria.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace SQL;
using namespace soci;

Criteria::Criteria(Table table) : table(table), n_to_skip(0), n_to_return(0)
{
}

unsigned int Criteria::count()
{
  table.sql << ("SELECT COUNT(id) FROM " + table.get_name() + where);
}

void Criteria::remove()
{
  table.sql << ("DELETE * FROM " + table.get_name() + where);
}

void Criteria::remove_one()
{
  table.sql << ("DELETE * FROM " + table.get_name() + where + " LIMIT 1");
}

void Criteria::update(const vector<IField*>& fields) const
{
  std::string query;
  bool        need_coma = false;
  
  query.reserve(12 + table.get_name().length() + where.length() + fields.length() * 15);
  query = "UPDATE " + table.get_name() + " SET ";
  for (auto it = fields.begin() ; it != fields.end() ; ++it)
  {
    if (need_coma)
      query += ',';
    if ((*it)->has_changed)
    {
      query += (*it)->key + '=' + (*it)->to_string();
      need_coma = true;
    }
  }
  query += where;
  table.sql << query;
}

long Criteria::insert(const vector<IField*>& fields) const
{
  long        id;
  std::string query;
  bool        need_coma = false;

  query.reserve(21 + table.get_name().length() + fields.length() * 10);
  query = "INSERT INTO " + table.get_name() + " VALUES(";
  for (auto it = fields.begin() ; it != fields.end() ; ++it)
  {
    if (need_coma)
      query += ',';
    else
      need_coma = true;
    query += (*it)->to_string();
  }
  query += ')';
  table.sql << query;
  table.sql.get_last_insert_id(table.get_name(), id);
  return id;
}

void Criteria::fetch_results() const
{
  std::string query;

  query.reserve(30 + table.get_name().length() + where.length() + order_items.length() * 6);
  query = "SELECT * FROM " + table.get_name() + where;
  if (n_to_skip >= 0 && n_to_return > 0)
    query += " LIMIT " + boost::lexical_cast<string>(n_to_skip) + ',' + boost::lexical_cast<string>(n_to_return);
  if (order_items.length() > 0)
    query = append_order_by_to_query(query);
  results = (table.sql.prepare << query);
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

Criteria& Criteria::or()
{
  where.reserve(where.length() + 6);
  where.insert(0, "(");
  where += ") OR ";
}

template<>
Criteria& Criteria::where<std::string>(const std::string& key, Operator op, const std::string& value)
{
  const unsigned char estimated_value_length = value.length() * 1.1 + 4;

  where.reserve(where.length() + key.length() + estimated_value_length + 12);
  where += (where.length() == 0) ? " WHERE " : " AND ";
  where += key + ' ' + operator_to_string(op) + " \"";
  for (std::string::size_type i = 0; i < value.length(); ++i) {
    switch (value[i])
    {
    case '"':
    case '\\':
      where += '\\';
    default:
      where += value[i];
    }
  }
  where += '"';
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
    case Like:                 return "LIKE";
  }
  return "=";
}