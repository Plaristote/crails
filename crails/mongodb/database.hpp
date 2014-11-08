#ifndef  MONGODB_DATABASE_HPP
# define MONGODB_DATABASE_HPP

# include <crails/databases.hpp>
# include "collection.hpp"

namespace MongoDB
{
  class Database : public Databases::Db
  {
  public:
    typedef std::list<Collection> Collections;

    static const std::string ClassType(void) { return ("mongodb"); }

    Database(const std::string& name, const std::string& host, unsigned short port);

    bool                       operator==(const std::string& name) const { return (this->name == name); }
    Collection&                operator[](const std::string& name);
    void                       AuthenticateWith(const std::string& username, const std::string& password);
    const std::string&         GetName(void)        const       { return (name);        }
    Collections&               GetCollections(void)             { return (collections); }
    const Collections&         GetCollections(void) const       { return (collections); }
    void                       RefreshCollections(void);

    void                       Connect(void);

  private:
    mongo::DBClientConnection connection;
    const std::string         name;
    Collections               collections;
    std::string               hostname, username, password;
    unsigned short            port;
    bool                      connected;
  };
}

#endif
