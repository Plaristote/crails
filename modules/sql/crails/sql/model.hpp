#ifndef  SQL_MODEL_HPP
# define SQL_MODEL_HPP

# include <crails/sql/database.hpp>

# define SQL_STORE_IN(db_name, table_name) \
  static const std::string DatabaseName(void)   { return (#db_name);    } \
  static const std::string CollectionName(void) { return (#table_name); }  \
  static SQL::Table        GetTable(void) { return CRAILS_DATABASE(SQL,#db_name)[#table_name]; } \
  static void              DatabaseMigrate(void);

# define SQL_MODEL(classname) \
  classname(); \
  classname(soci::row&); \
  void initialize_fields();

# define SQL_FIELD(type,fname,default) \
  SQL::Field<type> field_##fname; \
  \
  type get_##fname (void) const \
  { \
    return (field_##fname.value); \
  } \
  \
  void set_##fname (const type& cpy) \
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
  struct Field : public IField
  {
    Field(const std::string& key, const T& value) : IField(key), value(value)
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
      for (size_t i = 0 ; i < row.size() ; ++i)
      {
        if (key == row.get_properties(i).get_name())
        {
          value = row.get<T>(i);
          break ;
        }
      }
    }

    T value;
  };

  template<>
  struct Field<std::string> : public IField
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
      for (size_t i = 0 ; i < row.size() ; ++i)
      {
        if (key == row.get_properties(i).get_name())
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
    Model(Table table);
    Model(Table table, soci::row& row);

    void add_field(IField& field);

  public:
    virtual ~Model() {}

    bool         exists(void) const
    {
      return (initialized);
    }

    unsigned long Id(void) const { return (get_id()); }

    SQL_FIELD(long, id, 0)

    void save(void)
    {
      initialized ? update() : insert();
    }

  private:
    void insert(void);
    void update(void);

  protected:
    Table                table;
    std::vector<IField*> fields;
    bool                 initialized;
    soci::row*           row_ptr;
  };
}

#endif
