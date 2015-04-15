#ifndef  DATABASES_HPP
# define DATABASES_HPP

# include <Boots/Utils/singleton.hpp>
# include <Boots/Utils/datatree.hpp>
# include <vector>

# define CRAILS_DATABASE(type,database) \
  Databases::singleton::Get()->GetDatabase<type::Database>(database)

class Databases
{
  friend class ThreadSingleton<Databases>;
public:
  typedef ThreadSingleton<Databases> singleton;

  class Db
  {
    friend class Databases;
  public:
    Db(const std::string& type) : type(type) {}
    virtual ~Db();

    bool              operator==(const std::string& name) const { return (this->name == name); }
    const std::string Type(void) { return (type); }
    virtual void      Connect(void) = 0;

  protected:
    std::string       name;
    std::string       type;
  };

  typedef std::vector<Db*> Dbs;

  struct Exception : public std::exception
  {
    Exception(const std::string& message) : message(message) {}

    const char* what() const throw() { return (message.c_str()); }
    std::string message;
  };

private:
  Databases()
  {
    config_file = DataTree::Factory::JSON("../config/db.json");
    if (!(config_file))
      throw Databases::Exception("Couldn't load databases configuration file.");
  }

  ~Databases()
  {
    CleanupDatabases();
    if (config_file)
      delete config_file;
  }

  void CleanupDatabases();

public:
  Db* GetDatabaseFromName(const std::string& key);

  template<typename TYPE>
  Db* InitializeDatabase(const std::string& key)
  {
    Data  settings(config_file);
    TYPE* database       = new TYPE(settings[key]);
    Db*   database_as_db = database;

    database_as_db->name = key;
    databases.push_back(database);
    return (database);
  }

  template<typename TYPE>
  TYPE& GetDatabase(const std::string& key)
  {
    Db* db = GetDatabaseFromName(key);

    if (!db)
      db = InitializeDatabase<TYPE>(key);
    if (db->Type() != TYPE::ClassType())
      throw Databases::Exception("Expected type '" + TYPE::ClassType() + "', got '" + db->Type() + '\'');
    db->Connect();
    return (*(reinterpret_cast<TYPE*>(db)));
  }

private:
  DataTree*              config_file;
  Dbs                    databases;
};

#endif
