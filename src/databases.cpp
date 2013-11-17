#include "crails/databases.hpp"

#include "crails/mongodb.hpp"
#include "crails/sql.hpp"

using namespace std;

void Databases::LoadDatabases(void)
{
  Data             config(config_file);
  const Factories* rackables = Factories::singleton::Get();

  for_each(config.begin(), config.end(), [this, &rackables](Data db_config)
  {
    string               type   = db_config["type"].Value();
    Databases::DbFactory loader = rackables->GetByName(type);
    
    if (loader)
    {
      Databases::Db* db = loader(db_config);

      db->name = db_config.Key();
      databases.push_back(db);
    }
    else
      cout << "Failed to load database " << db_config.Key() << endl;
  });
}
