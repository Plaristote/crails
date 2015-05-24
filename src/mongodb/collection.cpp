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
