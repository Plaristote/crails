#ifndef  DATABASES_HPP
# define DATABASES_HPP

# include <Boots/Utils/singleton.hpp>
# include <Boots/Utils/datatree.hpp>
# include <vector>

class Databases
{
private:
  friend class ThreadSingleton<Databases>;
public:
  typedef ThreadSingleton<Databases> singleton;
public:  
  class Db
  {
    friend class Databases;
  public:
    Db(const std::string& type) : type(type)
    {}

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
    LoadDatabases();
  }
  
  ~Databases()
  {
    if (config_file)
      delete config_file;
  }
  
public:
  template<typename TYPE>
  TYPE& GetDb(const std::string& key)
  {
    Dbs::iterator it, end;
    Db*           db = 0;
    
    for (it = databases.begin(), end = databases.end() ; it != end ; ++it)
    {
      if (**it == key)
      {
        db = *it;
        break ;
      }
    }
    if (!db)
      throw Databases::Exception("Can't find the database named '" + key + '\'');
    if (db->Type() != TYPE::ClassType())
      throw Databases::Exception("Expected type '" + TYPE::ClassType() + "', got '" + db->Type() + '\'');
    db->Connect();
    return (*(reinterpret_cast<TYPE*>(db)));
  }

private:
  struct ConfigLoader
  {
    ConfigLoader(const std::string& type, std::function<void (Data)> loader) : type(type), loader(loader)
    {}

    std::string                type;
    std::function<void (Data)> loader;
  };
  
  typedef std::vector<ConfigLoader> ConfigLoaders;
  
  void LoadDatabases(void);
  
  DataTree* config_file;
  Dbs       databases;
};

#endif