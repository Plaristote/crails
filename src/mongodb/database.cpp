#include "crails/mongodb/database.hpp"
#include "crails/databases.hpp"

using namespace MongoDB;
using namespace std;

Database::Database(Data settings) :
  Db(ClassType()),
  name    (settings["database"].Value()),
  hostname(settings["host"].Value()),
  port    (settings["port"].Nil() ? 27017 : settings["port"])
{
  if (settings["username"].NotNil())
    AuthenticateWith(settings["username"].Value(), settings["password"].Value());
  connected = false;
}

void Database::AuthenticateWith(const std::string& username, const std::string& password)
{
  this->username = username;
  this->password = password;
}

void Database::Connect(void)
{
  if (!connected)
  {
    std::stringstream stream;
    std::string       err;

    stream << hostname << ':' << port;
    if (connection.connect(stream.str(), err))
    {
      std::cout << "[MongoDB] New connection to " << stream.str() << std::endl;
      RefreshCollections();
      connected = true;
    }
    else
      throw Databases::Exception(err);
  }
}

void Database::RefreshCollections(void)
{
  list<string> collection_fetch = connection.getCollectionNames(name);
  
  {
    Collections::iterator it  = collections.begin();
    Collections::iterator end = collections.end();
  
    while (it != end)
    {
      list<string>::const_iterator match = std::find(collection_fetch.begin(), collection_fetch.end(), it->GetFullName());
      
      if (match != collection_fetch.end())
        ++it;
      else
        it = collections.erase(it);
    }
  }
  
  {
    list<string>::const_iterator it  = collection_fetch.begin();
    list<string>::const_iterator end = collection_fetch.end();
    
    for (; it != end ; ++it)
    {
      const string name  = it->substr(this->name.size() + 1);
      auto         match = std::find(collections.begin(), collections.end(), name);

      if (match == collections.end())
        collections.push_back(Collection(connection, *this, name));
    }
  }
}

Collection& Database::operator[](const string& name)
{
  auto it = std::find(collections.begin(), collections.end(), name);

  if (it == collections.end())
  {
    collections.push_back(Collection(connection, *this, name));
    it = --(collections.end());
  }
  return (*it);
}
