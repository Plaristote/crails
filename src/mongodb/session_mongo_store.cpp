# include <Boots/include/Utils/datetime.hpp>
# include "crails/session_store/mongodb.hpp"

using namespace MongoDB;
using namespace std;

MongoStore::MongoStore()
{
}

void MongoStore::Load(Data request_headers)
{
  string cookie_string = request_headers["Cookie"].Value();

  cookie.Unserialize(cookie_string);
  if (cookie["session_id"].NotNil())
  {
    cout << "[MongoStore] Session Id = " << cookie["session_id"].Value() << endl;
    session = SessionStore::Find(cookie["session_id"].Value());
    if (session.NotNull())
      session->GetFields(session_content);
    else
      cout << "[MongoStore] Could not find the session object in the database" << endl;
  }
}

void MongoStore::Finalize(BuildingResponse& response)
{
  if (session.Null())
    session = new SessionStore(SessionStore::Create());
  session->SetFields(session_content);
  session->Save();
  while (cookie.Count())
    cookie[0].Remove();
  cookie["session_id"] = session->Id();
  response.SetHeaders("Set-Cookie", cookie.Serialize());
  MongoStore::SessionStore::Cleanup();
}

void MongoStore::SessionStore::GetFields(Data data)
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

void MongoStore::SessionStore::Save(void)
{
  Collection& collection = MONGODB_GET_COLLECTION(database_name, collection_name);

  if (has_id)
    collection.Update(bson_object, MONGO_QUERY("_id" << id));
  else
  {
    mongo::BSONElement id_element;

    collection.Insert(bson_object);
    if ((has_id = bson_object.getObjectID(id_element)))
      id = id_element.OID();
  }
}

void MongoStore::SessionStore::SetFields(Data data)
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
//  cout << "Set session: ";
//  data.Output();
}

void MongoStore::SessionStore::Cleanup(void)
{
  Collection&                                   collection  = MONGODB_GET_COLLECTION(database_name, collection_name);
  unsigned int                                  timestamp   = DateTime::Now();
  unsigned int                                  expiring_at = timestamp - session_expiration;

  collection.Remove(BSON("_updated_at" << mongo::LT << expiring_at));
}
