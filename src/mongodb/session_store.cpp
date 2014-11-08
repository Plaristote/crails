#include "crails/session_store/mongodb.hpp"

using namespace std;

MongoStore::SessionStore::SessionStore(MongoDB::Collection& collection, mongo::BSONObj bson_object) : Model(collection, bson_object)
{
}

MongoStore::SessionStore::SessionStore(const SessionStore& copy) : Model(copy)
{
}

SmartPointer<MongoStore::SessionStore> MongoStore::SessionStore::Find(const string& id_string)
{
  mongo::OID id;

  if (id_string.size() != 24)
    throw MongoDB::Exception(string("Invalid call to SessionStore::Find with OID '") + id_string + '\'');
  id.init(id_string);
  {
    SmartPointer<MongoDB::ResultSet<SessionStore> > results = MongoDB::ResultSet<SessionStore>::Query(MONGO_QUERY("_id" << id));
    SmartPointer<SessionStore>                      result(nullptr);

    if (results->Entries().size() > 0)
      result = new SessionStore(results->Entries().front());
    return (result);
  }
}

MongoStore::SessionStore MongoStore::SessionStore::Create(Data params)
{
  const string         database        = SessionStore::DatabaseName();
  const string         collection_name = SessionStore::CollectionName();
  MongoDB::Collection& collection      = Databases::singleton::Get()->GetDb<MongoDB::Database>(database)[collection_name];
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
