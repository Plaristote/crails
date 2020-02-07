#include "crails/mongodb/database.hpp"
#include "crails/mongodb/exception.hpp"
#include "crails/databases.hpp"

using namespace Crails;
using namespace MongoDB;
using namespace std;

Database::Database(const Databases::DatabaseSettings& settings) :
  Db(ClassType()),
  database_uri(uri_from_settings(settings)),
  database_name(Crails::any_cast(settings.at("name")))
{
}

void Database::connect()
{
  if (client == nullptr)
  {
    client   = unique_ptr<mongocxx::client>(new mongocxx::client(database_uri));
    database = client->database(database_name);
  }
}

void Database::drop()
{
  connect();
  database.drop();
}

mongocxx::uri Database::uri_from_settings(const Crails::Databases::DatabaseSettings& settings)
{
  std::stringstream url("mongodb://");

  url << defaults_to<const char*>(settings, "hostname", "localhost")
      << ':'
      << defaults_to<unsigned short>(settings, "port", 27017);
  return mongocxx::uri(url.str().c_str());
}

mongocxx::collection Database::get_collection(const std::string& name)
{
  connect();
  return database.collection(name);
}
