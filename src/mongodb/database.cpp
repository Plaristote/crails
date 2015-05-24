#include "crails/mongodb/database.hpp"
#include "crails/mongodb/exception.hpp"
#include "crails/databases.hpp"

using namespace Crails;
using namespace MongoDB;
using namespace std;

Database::Database(Data settings) :
  Db(ClassType()),
  name    (settings["database"].Value()),
  hostname(settings["host"].Value()),
  port    (settings["port"].Nil() ? 27017 : settings["port"])
{
  initialize_mongo_client();
  if (settings["username"].NotNil())
    authenticate_with(settings["username"].Value(), settings["password"].Value());
  connected = false;
}

void Database::initialize_mongo_client()
{
  static bool initialized = false;

  if (initialized == false)
  {
    mongo::client::Options options;
    mongo::Status          status = mongo::client::initialize(options);

    if (status.isOK())
      initialized = true;
    else
      throw MongoDB::Exception("failed to initialize mongo client");
  }
}

void Database::authenticate_with(const std::string& username, const std::string& password)
{
  this->username = username;
  this->password = password;
}

void Database::connect(void)
{
  if (!connected)
  {
    std::stringstream stream;
    std::string       err;

    stream << hostname << ':' << port;
    if (connection.connect(stream.str(), err))
    {
      std::cout << "[MongoDB] New connection to " << stream.str() << std::endl;
      refresh_collections();
      connected = true;
    }
    else
      throw Databases::Exception(err);
  }
}

void Database::drop_collection(const std::string& name)
{
  connection.dropCollection(name);
  refresh_collections();
}

void Database::refresh_collections(void)
{
  list<string> collection_fetch = connection.getCollectionNames(name);
  
  {
    Collections::iterator it  = collections.begin();
    Collections::iterator end = collections.end();
  
    while (it != end)
    {
      list<string>::const_iterator match = std::find(collection_fetch.begin(), collection_fetch.end(), it->get_full_name());
      
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
