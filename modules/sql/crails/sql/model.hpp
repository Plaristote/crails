#ifndef  SQL_MODEL_HPP
# define SQL_MODEL_HPP

# include <crails/sql/table.hpp>
# include <crails/sql/criteria.hpp>

# define TABLE(db_name, table_name) \
  static const std::string DatabaseName(void)   { return (#db_name);    } \
  static const std::string CollectionName(void) { return (#table_name); }  \
  static SQL::Table        GetTable(void) { return CRAILS_DATABASE(SQL,#db_name)[#table_name]; } \
  \
  static void DbMigrate() \
  { \
    SQL::Database& db = CRAILS_DATABASE(SQL,#db_name); \
    SQL::TableDescription desc = db.Describe(#table_name); \
    std::vector<SQL::TableDescription::Field> fields = { \
      SQL::TableDescription::Field("id", "int primary key auto_increment"),

# define TABLE_FIELD(name, type) \
    SQL::TableDescription::Field(#name, type)

# define END_TABLE \
    }; \
    desc.SetTableSchema(fields); \
  }

# define SQL_FIELD(type,fname,default) \
  Field<type> field_##fname; \
  \
  type get_fname (void) const \
  { \
    return (field_##fname.value); \
  } \
  \
  void set_##fname (const type cpy&) \
  { \
    field_##fname.value = cpy; \
  }

namespace SQL
{
  struct IField
  {
    IField(const std::string& key) : key(key), has_changed(false)
    {
    }

    const std::string   key;
    bool                has_changed;
    virtual std::string get_string() = 0;
    virtual void        initialize(soci::row&) = 0;
  };

  template<typename T>
  struct Field
  {
    Field(const std::string& key, T& value) : IField(key), value(value)
    {
    }

    std::string get_string()
    {
      std::stringstream stream;

      stream << value;
      return stream.str();
    }
    
    void initialize(soci::row& row)
    {
      for (short i = 0 ; i < row.size() ; ++i)
      {
        if (row.get_properties(i).get_name())
        {
          value = row.get<T>(i);
          break ;
        }
      }
    }

    T value;
  }

  template<>
  struct Field<std::string>
  {
    Field(const std::string& key, const std::string& value) : IField(key), value(value)
    {
    }

    std::string value;

    std::string get_string()
    {
      std::string out;

      out.reserve(value.length() * 1.1 + 4);
      out += '"';
      for (std::string::size_type i = 0; i < value.length(); ++i) {
        switch (value[i])
        {
        case '"':
        case '\\':
          out += '\\';
        default:
          out += value[i];
        }
      }
      out += '"';
      return out;
    }

    void initialize(soci::row& row)
    {
      for (short i = 0 ; i < row.size() ; ++i)
      {
        if (row.get_properties(i).get_name())
        {
          value = row.get<std::string>(i);
          break ;
        }
      }
    }
  };
  
  class Model
  {
  protected:
    Model(Table table) :
      field_id("id", 0), table(table), row_ptr(0), initialized(false)
    {
      add_field(field_id);
    }

    Model(Table table, soci::row& row) :
      field_id("id", 0), table(table), row_ptr(&row), initialized(true)
    {
      add_field(field_id);
    }

    void add_field(IField* field)
    {
      if (row_ptr)
        field->initialize(*row_ptr);
      fields.push_back(field);
    }

  public:
    bool         exists(void) const
    {
      return (initialized);
    }

    long Id(void) const { return (id()); }

    SQL_FIELD(long, id, 0)

    void save(void)
    {
      initialized ? update() : insert();
    }

  private:
    void insert(void)
    {
      Criteria criteria(table);

      field_id.value = criteria.insert(fields);
      if (field_id.value != 0)
        initialized = true;
    }

    void update(void)
    {
      Criteria criteria(table);
      
      criteria.where("id", Equal, Id());
      criteria.update(fields);
    }

  protected:    
    Table                table;
    std::vector<IField*> fields;
    bool                 initialized;
    soci::row*           row_ptr;
  };
}
    
#endif
