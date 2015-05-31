#include "crails/mongodb/collection.hpp"
#include "crails/mongodb/database.hpp"

using namespace MongoDB;
using namespace std;

const string Collection::get_full_name(void) const
{
  return (db.get_name() + '.' + name);
}

unsigned int Collection::count(const mongo::Query& query)
{
  return (connection.count(get_full_name(), query));
}

auto_ptr<mongo::DBClientCursor> Collection::query(mongo::Query query, int n_to_return, int n_to_skip, mongo::BSONObj* fields_to_return, int query_options, int batch_size)
{
  return (connection.query(get_full_name(), query, n_to_return, n_to_skip, fields_to_return, query_options, batch_size));
}

void Collection::find(std::vector<mongo::BSONObj>& out, mongo::Query query, int n_to_return, int n_to_skip, mongo::BSONObj* fields_to_return, int query_options)
{
  connection.findN(out, get_full_name(), query, n_to_return, n_to_skip, fields_to_return, query_options);
}

void Collection::update(mongo::BSONObj object, mongo::Query query, bool upsert, bool multi)
{
  connection.update(get_full_name(), query, object, upsert, multi);
}

void Collection::insert(mongo::BSONObj object, unsigned int flags)
{
  connection.insert(get_full_name(), object, flags);
}

void Collection::remove(const mongo::Query& query, bool just_one)
{
  connection.remove(get_full_name(), query, just_one);
}