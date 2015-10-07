#ifndef  MONGODB_DATABASE_HPP
# define MONGODB_DATABASE_HPP

# include <crails/databases.hpp>
# include "collection.hpp"

# define MONGODB_GET_COLLECTION(database,collection) \
  CRAILS_DATABASE(MongoDB,database)[collection]

namespace MongoDB
{
  class Database : public Crails::Databases::Db
  {
  public:
    typedef std::list<Collection> Collections;

    static const std::string ClassType(void) { return ("mongodb"); }

    Database(Data settings);

    bool                       operator==(const std::string& name) const { return (this->name == name); }
    Collection&                operator[](const std::string& name);
    void                       authenticate_with(const std::string& username, const std::string& password);
    const std::string&         get_name(void)         const      { return (name);        }
    Collections&               get_collections(void)             { return (collections); }
    const Collections&         get_collections(void) const       { return (collections); }
    void                       refresh_collections(void);
    void                       drop_all_collections(void);
    void                       drop_collection(const std::string& name);

    void                       connect(void);

  private:
    void                       initialize_mongo_client();

    mongo::DBClientConnection  connection;
    const std::string          name;
    Collections                collections;
    std::string                hostname, username, password;
    unsigned short             port;
    bool                       connected;
  };
}

#endif
