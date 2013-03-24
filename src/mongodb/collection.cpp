#include "crails/mongodb/collection.hpp"
#include "crails/mongodb/database.hpp"

using namespace MongoDB;
using namespace std;

const string Collection::GetFullName(void) const
{
  return (db.GetName() + '.' + name);
}

unsigned int Collection::Count(mongo::BSONObj query)
{
  return (connection.count(GetFullName(), query));
}
