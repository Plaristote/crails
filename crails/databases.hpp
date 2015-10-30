#ifndef  DATABASES_HPP
# define DATABASES_HPP

# include <vector>
# include "datatree.hpp"
# include "environment.hpp"

# define CRAILS_DATABASE(type,database) \
  Crails::databases.get_database<type::Database>(database)

namespace Crails
{
  class Databases
  {
  public:
    class Db
    {
      friend class Databases;
    public:
      Db(const std::string& type) : type(type) {}
      virtual ~Db();

      bool              operator==(const std::string& name) const { return (this->name == name); }
      const std::string get_type(void) { return (type); }
      virtual void      connect(void) = 0;

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

    Databases()
    {
    }

    ~Databases()
    {
      cleanup_databases();
    }

    void cleanup_databases();

    Db* get_database_from_name(const std::string& key);

    template<typename TYPE>
    Db* initialize_database(const std::string& key)
    {
      Data  environment_settings = settings[Crails::environment];
      TYPE* database             = new TYPE(environment_settings[key]);
      Db*   database_as_db       = database;

      database_as_db->name = key;
      databases.push_back(database);
      return (database);
    }

    template<typename TYPE>
    TYPE& get_database(const std::string& key)
    {
      Db* db = get_database_from_name(key);

      if (!db)
        db = initialize_database<TYPE>(key);
      if (db->get_type() != TYPE::ClassType())
        throw Databases::Exception("Expected type '" + TYPE::ClassType() + "', got '" + db->get_type() + '\'');
      db->connect();
      return (*(reinterpret_cast<TYPE*>(db)));
    }

  private:
    DataTree settings;
    Dbs      databases;
  };

  extern thread_local Databases databases;
}

#endif
