#include "crails/mongodb/model.hpp"
#include "crails/mongodb/collection.hpp"

using namespace MongoDB;
using namespace std;

bool Model::get_oid_from_object(mongo::BSONObj object, mongo::OID& oid)
{
  mongo::BSONElement id_element;
  bool               has_oid;

  if ((has_oid = object.getObjectID(id_element)))
    oid = id_element.OID();
  return (has_oid);
}

Model::Model(Collection& collection, mongo::BSONObj bson_object) : bson_object(bson_object), collection(collection)
{
  has_id             = get_oid_from_object(bson_object, id);
  fields_initialized = false;
}

Model::Model(const Model& copy) : bson_object(copy.bson_object), id(copy.id), has_id(copy.has_id), collection(copy.collection)
{
  fields_initialized = false;
}

void Model::initialize_fields(void)
{
  fields.clear();
}

bool Model::refresh(void)
{
  auto_ptr<mongo::DBClientCursor> list = collection.query(MONGO_QUERY("_id" << id));
  
  if (list->more())
  {
    mongo::BSONObj object = list->next();
    
    bson_object = object;
    initialize_fields();
    return (true);
  }
  return (false);
}

mongo::BSONObj Model::update(void)
{
  mongo::BSONObjBuilder        builder;
  mongo::BSONObj               object;
  std::list<IField*>::iterator it  = fields.begin();
  std::list<IField*>::iterator end = fields.end();

  if (!(has_id)) { builder.genOID(); }
  for (; it != end ; ++it)
    (*it)->BuildUpdate(builder);
  object = builder.obj();
  if (!(has_id))
  {
    has_id = get_oid_from_object(object, id);
    return (object);
  }
  return (BSON("$set" << object));
}

void Model::save(void)
{
  if (has_id)
  {
    mongo::Query query = MONGO_QUERY("_id" << id);

    collection.update(update(), query);
  }
  else
  {
    force_update();
    collection.insert(update());
  }
}

void Model::remove(void)
{
  if (has_id)
  {
    mongo::Query query = MONGO_QUERY("_id" << id);
    
    collection.remove(query, true);
    has_id = false;
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

void Model::set_foreign_id(const string& foreign_key, const mongo::OID id)
{
  GetField<mongo::OID>(foreign_key).Set(id);
}

void Model::set_owner_id(const string& relation_name, const string& id_string)
{
  if (id_string.size() == 24)
  {
    mongo::OID id; id.init(id_string);
    set_foreign_id(relation_name + "_id", id);
  }
  else
    throw MongoDB::Exception(std::string("Invalid call to set_") + relation_name + std::string("_id with OID '") + id_string + '\'');
}
