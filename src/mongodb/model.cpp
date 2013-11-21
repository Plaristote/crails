#include "crails/mongodb/model.hpp"
#include "crails/mongodb/collection.hpp"

using namespace MongoDB;
using namespace std;

bool Model::GetOidFromObject(mongo::BSONObj object, mongo::OID& oid)
{
  mongo::BSONElement id_element;
  bool               has_oid;

  if ((has_oid = object.getObjectID(id_element)))
    oid = id_element.OID();
  return (has_oid);
}

Model::Model(Collection& collection, mongo::BSONObj bson_object) : bson_object(bson_object), collection(collection), result_set(nullptr)
{
  has_id = GetOidFromObject(bson_object, id);
}

Model::Model(const Model& copy) : bson_object(copy.bson_object), id(copy.id), has_id(copy.has_id), collection(copy.collection), result_set(copy.result_set)
{
}

void Model::InitializeFields(void)
{
  fields.clear();
}

bool Model::Refresh(void)
{
  auto_ptr<mongo::DBClientCursor> list = collection.Query(QUERY("_id" << id));
  
  if (list->more())
  {
    mongo::BSONObj object = list->next();
    
    bson_object = object;
    InitializeFields();
    return (true);
  }
  return (false);
}

mongo::BSONObj Model::Update(void)
{
  mongo::BSONObjBuilder        builder;
  mongo::BSONObj               object;
  std::list<IField*>::iterator it  = fields.begin();
  std::list<IField*>::iterator end = fields.end();

  if (!(has_id)) { builder.genOID(); }
  cout << "Updating " << fields.size() << " fields." << endl;
  for (; it != end ; ++it)
    (*it)->BuildUpdate(builder);
  object = builder.obj();
  if (!(has_id))
  {
    has_id = GetOidFromObject(object, id);
    return (object);
  }
  return (BSON("$set" << object));
}

void Model::Save(void)
{
  if (has_id)
  {
    mongo::Query query = QUERY("_id" << id);
  
    collection.Update(Update(), query);
  }
  else
  {
    ForceUpdate();
    collection.Insert(Update());
  }
}

void Model::Remove(void)
{
  if (has_id)
  {
    mongo::Query query = QUERY("_id" << id);
    
    collection.Remove(query, true);
  }
}

template<>
void Model::Field<mongo::OID>::initialize(const std::string& key, mongo::BSONObj object, mongo::OID def)
{
  auto element = object.getField(key);

  if (element.ok())
    value      = element.OID();
  name         = key;
  has_changed  = false;
}

template<>
Model::Field<mongo::OID>::Field(const Model::Field<mongo::OID>& field)
{
  value       = field.value;
  has_changed = field.has_changed;
}
