# include <Boots/Utils/datetime.hpp>
# include "crails/session_store/mongodb.hpp"

using namespace Crails;
using namespace MongoDB;
using namespace std;

MongoStore::MongoStore()
{
}

void MongoStore::load(Data request_headers)
{
  string cookie_string = request_headers["Cookie"].Value();

  cookie.unserialize(cookie_string);
  if (cookie["session_id"].NotNil())
  {
    cout << "[MongoStore] Session Id = " << cookie["session_id"].Value() << endl;
    session = SessionStore::Find(cookie["session_id"].Value());
    if (session.NotNull())
      session->get_fields(session_content);
    else
      cout << "[MongoStore] Could not find the session object in the database" << endl;
  }
}

void MongoStore::finalize(BuildingResponse& response)
{
  bool is_session_empty = session_content.Count() <= 1 && session_content["flash"].Count() == 0;

  if (!session.Null() || !is_session_empty)
  {
    if (session.Null())
      session = new SessionStore(SessionStore::Create());
    session->set_fields(session_content);
    session->save();
    while (cookie.Count())
      cookie[0].Remove();
    cookie["session_id"] = session->Id();
    response.set_headers("Set-Cookie", cookie.serialize());
    MongoStore::SessionStore::Cleanup();
  }
}

void MongoStore::SessionStore::get_fields(Data data)
{
  set<string> field_names;

  bson_object.getFieldNames(field_names);
  {
    auto it  = field_names.begin();
    auto end = field_names.end();

    for (; it != end ; ++it)
    {
      std::string key   = *it;
      std::string value = bson_object.getField(key).str();

      if (key != "_id" && key != "_updated_at")
        data[key] = value;
    }
  }
}

void MongoStore::SessionStore::save(void)
{
  Collection& collection = MONGODB_GET_COLLECTION(database_name, collection_name);

  if (has_id)
    collection.update(bson_object, MONGO_QUERY("_id" << id));
  else
  {
    mongo::BSONElement id_element;

    collection.insert(bson_object);
    if ((has_id = bson_object.getObjectID(id_element)))
      id = id_element.OID();
  }
}

void MongoStore::SessionStore::set_fields(Data data)
{
  mongo::BSONObjBuilder builder;
  auto                  it         = data.begin();
  auto                  end        = data.end();
  unsigned int          updated_at = DateTime::Now();

  if (has_id)
    builder << "_id" << id;
  else
    builder.genOID();
  builder   << "_updated_at" << updated_at;
  for (; it != end ; ++it)
    builder << (*it).Key() << (*it).Value();
  bson_object = builder.obj();
}

void MongoStore::SessionStore::Cleanup(void)
{
  Collection&                                   collection  = MONGODB_GET_COLLECTION(database_name, collection_name);
  unsigned int                                  timestamp   = DateTime::Now();
  unsigned int                                  expiring_at = timestamp - session_expiration;

  collection.remove(BSON("_updated_at" << mongo::LT << expiring_at));
}
