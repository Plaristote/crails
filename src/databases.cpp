#include "crails/databases.hpp"

#include "include/mongodb.hpp"
#include "include/sql.hpp"

using namespace std;

void Databases::LoadDatabases(void)
{
  Data config(config_file);
  
  for_each(config.begin(), config.end(), [this](Data db_config)
  {
    string        type = db_config["type"].Value();
    ConfigLoaders config_loaders = {
      ConfigLoader("mongodb", [this](Data data)
      {
        databases.push_back(new MongodbDb(data));
        (*databases.rbegin())->name = data.Key();
      }),
      ConfigLoader("sql", [this](Data data)
      {
        databases.push_back(new SqlDb(data));
        (*databases.rbegin())->name = data.Key();
      })
    };

    for (unsigned short i = 0 ; i < 2 ; ++i)
    {
      if (type == config_loaders[i].type)
      {
        config_loaders[i].loader(db_config);
        break ;
      }
    }
  });
}