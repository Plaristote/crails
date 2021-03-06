#include "crails/session_store/mongodb.hpp"

using namespace std;
using namespace Crails;

MongoStore::SessionStore::SessionStore(MongoDB::Collection& collection, mongo::BSONObj bson_object) : Model(collection, bson_object)
{
}

MongoStore::SessionStore::SessionStore(const SessionStore& copy) : Model(copy)
{
}

MongoStore::~MongoStore()
{
}

SmartPointer<MongoStore::SessionStore> MongoStore::SessionStore::Find(const string& id_string)
{
  mongo::OID id;

  if (id_string.size() != 24)
    throw MongoDB::Exception(string("Invalid call to SessionStore::Find with OID '") + id_string + '\'');
  id.init(id_string);
  {
    SmartPointer<MongoDB::Criteria<SessionStore> > results = MongoDB::Criteria<SessionStore>::prepare(MONGO_QUERY("_id" << id));
    SmartPointer<SessionStore>                      result(nullptr);

    if (results->entries().size() > 0)
      result = new SessionStore(results->entries().front());
    return (result);
  }
}

MongoStore::SessionStore MongoStore::SessionStore::Create(Data params)
{
  const string         database        = SessionStore::DatabaseName();
  const string         collection_name = SessionStore::CollectionName();
  MongoDB::Collection& collection      = databases.get_database<MongoDB::Database>(database)[collection_name];
  mongo::BSONObj       obj;
  {
    mongo::BSONObjBuilder builder;

    if (!(params.Nil()))
    {
      for_each(params.begin(), params.end(), [&builder](Data param)
      {
        builder << param.Key() << param.Value();
      });
    }
    obj = builder.obj();
  }
  return (SessionStore(collection, obj));
}

// FIELDS
void MongoStore::SessionStore::initialize_fields()
{
  fields_initialized = true;
}
