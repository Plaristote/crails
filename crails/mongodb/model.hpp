#ifndef  MONGODB_MODEL_HPP
# define MONGODB_MODEL_HPP

# include <mongo/client/dbclient.h>
# include <Boots/Utils/smart_pointer.hpp>
# include "exception.hpp"

# define MONGODB_MODEL(classname) \
    friend class MongoDB::ResultSet<classname>; \
  public: \
    classname(MongoDB::Collection& collection, mongo::BSONObj& bson_object) : Model(collection, bson_object) \
    { \
      InitializeFields(); \
    } \
    \
    classname(const classname& copy) : Model(copy) \
    { \
      InitializeFields(); \
    } \
    \
    static SmartPointer<classname> Find(const std::string& id_str) \
    { \
      mongo::OID                                     id; \
      \
      if (id_str.size() != 24) \
        throw MongoDB::Exception(std::string("Invalid call to ") + std::string(#classname) + std::string("::Find with OID '") + id_str + '\''); \
      id.init(id_str); \
      { \
        SmartPointer<MongoDB::ResultSet<classname> > results = MongoDB::ResultSet<classname>::Query(QUERY("_id" << id)); \
        SmartPointer<classname>                      result(nullptr); \
  \
        if (results->Entries().size() > 0) \
          result = new classname(results->Entries().front()); \
        return (result); \
      } \
    } \
    \
    static classname Create(Data params = Data()) \
    { \
      const std::string    database        = classname::DatabaseName(); \
      const std::string    collection_name = classname::CollectionName(); \
      MongoDB::Collection& collection      = Databases::singleton::Get()->GetDb<MongodbDb>(database)[collection_name]; \
      mongo::BSONObj       obj; \
      { \
        mongo::BSONObjBuilder builder; \
        \
        if (!(params.Nil())) \
        { std::for_each(params.begin(), params.end(), [&builder](Data param) { builder << param.Key() << param.Value(); }); } \
        obj = builder.obj(); \
      } \
      return (classname(collection, obj)); \
    }

# define MONGODB_FIELD(type,field,def) \
private:  Field<type> _##field ; \
  inline void initialize_##field (void) { _##field.initialize(#field, bson_object, def); fields.push_back(&_##field); }\
public: \
  type field (void) const { return (_##field.Get()); }\
  void set_##field (type val) { _##field.Set(val); }

# define MONGODB_HAS_MANY_AS(collection_name,type,relation_name) \
  MongoDB::ResultSet<type>* relation_name##s (void) \
  { \
    std::string      association      = std::string(#collection_name) + 's'; \
    std::string      association_name = std::string(#relation_name)   + "_id"; \
    mongo::Query     query            = QUERY(association_name << OID()); \
    MongoDB::ResultSet<type>* ptr     = MongoDB::ResultSet<type>::Query(collection.Db()[association], query);\
    \
    ptr->SetOwner(*this, #relation_name);\
    return (ptr);\
  }

# define MONGODB_HAS_MANY(collection_name,type) \
  MONGODB_HAS_MANY_AS(collection_name,type,collection_name)

# define MONGODB_BELONGS_TO_AS(collection_name,type,relation_name) \
  MONGODB_FIELD(mongo::OID, relation_name##_id , mongo::OID()) \
  type relation_name(void) \
  {\
    mongo::OID            foreign_oid = relation_name##_id (); \
    mongo::Query          query = QUERY("_id" << foreign_oid); \
    MongoDB::Collection&  col   = GetCollection().Db()[std::string(#collection_name) + 's']; \
    auto                  ptr   = col.Query(query); \
    \
    if (ptr->more()) \
    { \
      type                  obj(col, ptr->next());\
      \
      obj.InitializeFields(); \
      return (obj); \
    } \
    return (type(col, mongo::BSONObj())); \
  }
  
# define MONGODB_BELONGS_TO(collection_name,type) \
  MONGODB_BELONGS_TO_AS(collection_name,type,collection_name)
  
namespace MongoDB
{
  class Collection;
  
  class Model
  {
  public:
    Model(Collection& collection, mongo::BSONObj bson_object);
    Model(const Model& copy);

    std::string        Id(void)  const { return (id.toString()); }
    mongo::OID         OID(void) const { return (id);            }
    virtual void       Save(void);
    virtual void       Remove(void);
    bool               Refresh(void);
    Collection&        GetCollection(void)       { return (collection); }
    const Collection&  GetCollection(void) const { return (collection); }

  private:
    static bool        GetOidFromObject(mongo::BSONObj object, mongo::OID& oid);
    mongo::BSONObj     Update(void);

  protected:
    virtual void       InitializeFields(void);

    void               ForceUpdate(void)
    {
      std::for_each(fields.begin(), fields.end(), [](IField* field) { field->ForceUpdate(); });
    }

    bool               Exists(void) const
    {
      return (has_id);
    }
    
    struct IField;

    mongo::BSONObj                       bson_object;
    mongo::OID                           id;
    std::list<IField*>                   fields;
    bool                                 has_id;
    Collection&                          collection;
    SmartPointer<mongo::DBClientCursor>  result_set; // Must hold reference to result set if was part of a result set

    /*
     * Field Management
     */
    struct IField
    {
      friend class Model;
    public:
      std::string  Name(void) { return (name); }
      virtual void ForceUpdate(void)                   = 0;
    protected:
      virtual void BuildUpdate(mongo::BSONObjBuilder&) = 0;

      std::string name;
    };

    template<typename TYPE>
    class Field : public IField
    {
    public:
      Field(void) {}
      
      Field(const Field& field)
      {
        value       = field.value;
        has_changed = field.has_changed;
      }
      
      void initialize(const std::string& key, mongo::BSONObj bson_object, TYPE def)
      {
        auto element = bson_object.getField(key);

        name    = key;
        if (!(element.ok()))
          value = def;
        else
          element.Val(value);
        has_changed = false;
      }
      
      void        ForceUpdate(void) { has_changed = true; }

      TYPE        Get(void) const        { return (value);      }
      void        Set(const TYPE& value)
      {
        this->value = value;
        has_changed = true;
      }

    protected:
      void        BuildUpdate(mongo::BSONObjBuilder& builder)
      {
        std::cout << "Updating field " << name << std::endl;
        if (has_changed)
        {
          std::cout << "Needs update" << std::endl;
          builder << name << value;
        }
      }
    private:
      TYPE         value;
      bool         has_changed;
    };    
  };
  
  template<>
  Model::Field<mongo::OID>::Field(const Model::Field<mongo::OID>& field);
  template<>
  void Model::Field<mongo::OID>::initialize(const std::string& key, mongo::BSONObj bson_object, mongo::OID def);
}

#endif
