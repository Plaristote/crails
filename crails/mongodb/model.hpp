#ifndef  MONGODB_MODEL_HPP
# define MONGODB_MODEL_HPP

# include <mongo/client/dbclient.h>
# include <Boots/Utils/smart_pointer.hpp>
# include <Boots/Utils/dynstruct.hpp>
# include "exception.hpp"
# include "array.hpp"
# include "collection.hpp"

# define MONGODB_COLLECTION(database,collection) \
  static std::string DatabaseName()        { return (#database);   } \
  static std::string CollectionName()      { return (#collection); } \
  static MongoDB::Collection& Collection() { return (MONGODB_GET_COLLECTION(#database,#collection)); }

# define MONGODB_MODEL(classname) \
  friend class MongoDB::ResultSet<classname>; \
  classname(Data params = Data()); \
  classname(MongoDB::Collection& collection, mongo::BSONObj bson_object); \
  classname(const classname& copy); \
  static SmartPointer<classname> Find(const std::string& id_str); \
  static SP(MongoDB::ResultSet<classname>) Where(mongo::Query query = mongo::Query()) \
  { return MongoDB::ResultSet<classname>::prepare(query); } \
  static classname Create(Data params = Data()); \
  DynStruct to_data() const; \
  void initialize_fields(void); \
  static mongo::BSONObj data_to_bson(Data);

# define MONGODB_REQUIRE(filename)

# define MONGODB_FIELD(type,field,def) \
  Field<type> field;\
  type get_##field (void) const;\
  void set_##field (type val);

# define MONGODB_HAS_ONE_AS(type,remote_field,relation_name) \
  SP(type) get_##relation_name(void) const;

# define MONGODB_HAS_ONE(type,remote_field) \
  MONGODB_HAS_ONE_AS(type,remote_field,type)

# define MONGODB_HAS_MANY_AS(type,remote_field,relation_name) \
  SP(MongoDB::ResultSet<type>) get_##relation_name##s (void) const;

# define MONGODB_HAS_MANY(type,remote_field) \
  MONGODB_HAS_MANY_AS(type,remote_field,type)

# define MONGODB_BELONGS_TO_AS(type,relation_name) \
  MONGODB_FIELD(mongo::OID, relation_name##_id , mongo::OID()) \
  void set_##relation_name##_id(const std::string& str); \
  SP(type) get_##relation_name(void) const;

# define MONGODB_BELONGS_TO(type) \
  MONGODB_BELONGS_TO_AS(type,type)

# define MONGODB_HAS_AND_BELONGS_TO_MANY_HOST(type,relation_name) \
  MONGODB_FIELD(MongoDB::Array<mongo::OID>, relation_name##_ids, MongoDB::Array<mongo::OID>()) \
  SP(MongoDB::ResultSet<type>) get_##relation_name##s (void) const; \
  void add_to_##relation_name##s (const type&); \
  void remove_from_##relation_name##s (const type&);

# define MONGODB_HAS_AND_BELONGS_TO_MANY(type,relation_name,foreign_name) \
  SP(MongoDB::ResultSet<type>) get_##relation_name##s (void) const; \
  void add_to_##relation_name##s (type&) const; \
  void remove_from_##relation_name##s (type&) const;

namespace MongoDB
{
  class Collection;

  class Model
  {
  public:
    Model(Collection& collection, mongo::BSONObj bson_object);
    Model(const Model& copy);
    virtual ~Model() {}

    std::string        Id(void)  const { return (id.toString()); }
    mongo::OID         OID(void) const { return (id);            }
    virtual void       save(void);
    virtual void       remove(void);
    bool               refresh(void);
    Collection&        get_collection(void)       { return (collection); }
    const Collection&  get_collection(void) const { return (collection); }

    bool               exists(void) const { return (has_id); }

  private:
    static bool        get_oid_from_object(mongo::BSONObj object, mongo::OID& oid);
    mongo::BSONObj     update(void);

  protected:
    virtual void       initialize_fields(void);

    void               set_owner_id(const std::string& relation_name, const std::string& id_string);
    void               force_update(void)
    {
      std::for_each(fields.begin(), fields.end(), [](IField* field) { field->ForceUpdate(); });
    }

    struct IField;

    mongo::BSONObj                       bson_object;
    mongo::OID                           id;
    std::list<IField*>                   fields;
    bool                                 has_id, fields_initialized;
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
      bool operator==(const std::string& comp) const { return (name == comp); }
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
        if (has_changed)
          builder << name << value;
      }
    private:
      TYPE         value;
      bool         has_changed;
    };

    template<typename Type>
    Field<Type>&       GetField(const std::string& fieldname) const
    {
      auto it = std::find_if(fields.begin(), fields.end(), [fieldname](IField* field)
      { return (*field == fieldname); });

      if (it == fields.end())
        throw MongoDB::Exception("No such field " + fieldname);
      return (*(reinterpret_cast<Field<Type>*>(*it)));
    }
  };

  template<>
  Model::Field<mongo::OID>::Field(const Model::Field<mongo::OID>& field);
  template<>
  void Model::Field<mongo::OID>::initialize(const std::string& key, mongo::BSONObj bson_object, mongo::OID def);
}

# include "resultset.hpp"

#endif
