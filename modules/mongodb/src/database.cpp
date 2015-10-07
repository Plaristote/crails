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
  port    (settings["port"].Nil() ? (unsigned short)27017 : (unsigned short)settings["port"])
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
    mongo::HostAndPort host_and_port(hostname, port);
    stringstream       stream;
    string             err;

    stream << hostname << ':' << port;
    connection.connect(mongo::HostAndPort(hostname, port), err);
    if (connection.connect(host_and_port, err))
    {
      refresh_collections();
      connected = true;
    }
    else
      throw Databases::Exception(err);
  }
}

void Database::drop_all_collections(void)
{
  for_each(collections.begin(), collections.end(), [this](Collection collection)
  {
    connection.dropCollection(collection.get_full_name());
  });
}

void Database::drop_collection(const std::string& name)
{
  connection.dropCollection(name);
}

void Database::refresh_collections(void)
{
  list<string> collection_fetch = connection.getCollectionNames(name);
  list<string>::const_iterator it  = collection_fetch.begin();
  list<string>::const_iterator end = collection_fetch.end();

  for (; it != end ; ++it)
  {
    if (it->length() > this->name.size() + 1)
    {
      const string name  = it->substr(this->name.size() + 1);
      auto         match = find(collections.begin(), collections.end(), name);

      if (match == collections.end())
        collections.push_back(Collection(connection, *this, name));
    }
  }
}

Collection& Database::operator[](const string& name)
{
  auto it = find(collections.begin(), collections.end(), name);

  if (it == collections.end())
  {
    collections.push_back(Collection(connection, *this, name));
    it = --(collections.end());
  }
  return (*it);
}
