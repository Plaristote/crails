#ifndef  MONGODB_DATABASE_HPP
# define MONGODB_DATABASE_HPP

# include "collection.hpp"

namespace MongoDB
{
  class Database
  {
  public:
    typedef std::list<Collection> Collections;

    Database(mongo::DBClientConnection& connection, const std::string& name);

    bool                       operator==(const std::string& name) const { return (this->name == name); }
    Collection&                operator[](const std::string& name);
    const std::string&         GetName(void)        const { return (name);        }
    Collections&               GetCollections(void)       { return (collections); }
    const Collections&         GetCollections(void) const { return (collections); }
    void                       RefreshCollections(void);

  private:
    mongo::DBClientConnection& connection;
    const std::string          name;
    Collections                collections;
  };
}

#endif