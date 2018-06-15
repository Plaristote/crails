#include "../crails/odb/migration.hpp"

using namespace ODB;
using namespace std;

bool Migrations::run_for_version(ODB::Database& db, odb::schema_version version) const
{
  for (auto migration : list)
  {
    if (migration.version == version)
    {
      cout << "+ " << migration.name << endl;
      if (!(migration.runner(db)))
      {
        cout << "/!\\ Migration failed to run" << endl;
        return false;
      }
    }
  }
  return true;
}
