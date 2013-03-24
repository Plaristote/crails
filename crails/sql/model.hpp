#ifndef  SQL_MODEL_HPP
# define SQL_MODEL_HPP

# include "table.hpp"

# define TABLE(db_name, table_name) \
  static const std::string DatabaseName(void)   { return (#db_name);    } \
  static const std::string CollectionName(void) { return (#table_name); }  \
  \
  static void DbMigrate(SQL::Database& db) \
  { \
    SQL::TableDescription desc = db.Describe(#table_name); \
    std::vector<SQL::TableDescription::Field> fields = { \
      SQL::TableDescription::Field("id", "int primary key auto_increment"),

# define TABLE_FIELD(name, type) \
    SQL::TableDescription::Field(#name, type)

# define END_TABLE \
    }; \
    desc.SetTableSchema(fields); \
  }
  
# define SQL_FIELD_WITH_VISIBILITY(type,fname,default,visibility) \
  private:\
    Field<type> field_##fname; \
  visibility:\
    type fname (void) const \
    { \
      return (field_##fname.Get()); \
    } \
    \
    void set_##fname (const type cpy) \
    { \
      field_##fname.Set(cpy); \
    } \
    \
  protected:\
    void initialize_##fname (void) \
    { \
      if (row_ptr) \
      { \
        soci::row& row = *row_ptr; \
        \
        if (current_field_it < row.size()) \
        { \
          field_##fname.value = row.get<type>(current_field_it); \
          field_##fname.name  = row.get_properties(current_field_it).get_name(); \
          current_field_it++; \
          fields.push_back(&field_##fname); \
        } \
        initialized = true; \
      } \
      else \
      { \
        field_##fname.value = default; \
        field_##fname.name  = #fname; \
        current_field_it++; \
        fields.push_back(&field_##fname); \
        initialized = false; \
      } \
    } \
    \
  public:

# define SQL_FIELD(type,fname,default) \
  SQL_FIELD_WITH_VISIBILITY(type,fname,default,public)

namespace SQL
{
  class Model
  {
  public:
    Model(Table table) : table(table)
    {
      current_field_it = 0;
      row_ptr          = 0;
      initialized      = false;
      initialize_id();
    }
    
    Model(Table table, soci::row& row) : table(table)
    {
      current_field_it = 0;
      row_ptr          = &row;
      initialized      = false;
      initialize_id();
    }

    virtual void InitializeFields(void) = 0;

    unsigned int Id(void) const { return (id()); }

    struct IField
    {
      virtual void AppendUpdate(std::stringstream&) = 0;
      virtual void AppendInsert(std::stringstream&) = 0;
      
      std::string name;
      std::string type;
      bool        has_changed;
    };
    
    template<typename T>
    struct Field : public IField
    {
      Field(void)
      {
        has_changed = false;
      }
      
      virtual void AppendUpdate(std::stringstream& stream)
      {
        stream << name << "='" << value << "' ";
      }
      
      virtual void AppendInsert(std::stringstream& stream)
      {
        stream << '"' << value << '"';
      }

      T    Get(void) const { return (value); }
      void Set(const T& cpy) { value = cpy; has_changed = true; }

      T    value;
    };
    
    SQL_FIELD_WITH_VISIBILITY(int, id, 0, private)
    
    void Save(void)
    {
      initialized ? Update() : Insert();
    }
    
  private:
    void Insert(void)
    {
      std::stringstream query;
      auto it = fields.begin();
      auto end = fields.end();
      int  count = 0;
      
      query << "insert into " << table.GetName() << " values(";
      for (; it != end ; ++it)
      {
        if (count)
          query << ',';
        (*it)->AppendInsert(query);
        ++count;
      }
      query << ')';
      table.Query(query.str());
      table.Query("select LAST_INSERT_ID() from " + table.GetName() + " limit 0,1", field_id.value);
      initialized = true;
    }
    
    void Update(void)
    {
      std::stringstream query;
      auto it    = fields.begin();
      auto end   = fields.end();
      int  count = 0;
      
      query << "update " << table.GetName();
      for (; it != end ; ++it)
      {
        if ((*it)->has_changed)
        {
          if (!count)
            query << " set ";
          else
            query << " and ";
          (*it)->AppendUpdate(query);
          ++count;
        }
      }
      query << " where id='" << Id() << '\'';
      if (count)
        table.Query(query.str());      
    }
    
  protected:    
    unsigned short       current_field_it;
    Table                table;
    std::vector<IField*> fields;
    bool                 initialized;
    soci::row*           row_ptr;
  };
}
    
#endif