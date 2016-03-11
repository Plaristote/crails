#include <crails/odb/database.hpp>
#include <odb/schema-catalog.hxx>

using namespace std;

void migrate_database(odb::database& database)
{
  odb::schema_version v(database.schema_version());
  odb::schema_version cv(odb::schema_catalog::current_version(database));
 
  if (v == 0)
  {
    odb::transaction t(database.begin());
    cout << ":: No schema version found." << endl;
    cout << ":: Initializing database from scratch" << endl;
    odb::schema_catalog::create_schema(database);
    t.commit();
  }
  else if (v < cv)
  {
    cout << ":: Migration" << endl;
    for (v = odb::schema_catalog::next_version(database, v) ;
         v <= cv ;
         v = odb::schema_catalog::next_version(database, v))
    {
      odb::transaction t(database.begin());

      cout << ":: Running migration to version " << v << endl;
      odb::schema_catalog::migrate_schema_pre(database, v);
      // TODO run migrations here
      odb::schema_catalog::migrate_schema_post(database, v);
      t.commit();
    }
  }
  else
    cout << ":: Nothing to do" << endl;
}

odb::database& get_database(char* name)
{
  auto& crails_database = CRAILS_DATABASE(ODB, name);

  return crails_database.get_agnostic_database();
}

int help(char* arg_command)
{
  cout << "Usage: " << arg_command << " [database_key]" << endl;
  return -1;
}

int main(int argc, char** argv)
{
  if (argc < 2)
    return help(argv[0]);
  else
    migrate_database(get_database(argv[1]));
  return 0;
}
