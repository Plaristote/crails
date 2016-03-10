#include <crails/odb/database.hpp>
#include <odb/schema-catalog.hxx>
#include <odb/pgsql/database.hxx>

#ifndef sql_backend
# define sql_backend pgsql
#endif

using namespace std;

template<typename T>
void migrate_database(T& database)
{
  odb::transaction t(database.begin());

  odb::schema_catalog::create_schema(database);
  t.commit();
}

odb::sql_backend::database& get_database(char* name)
{
  auto& crails_database = CRAILS_DATABASE(ODB, name);

  return crails_database.get_database<odb::sql_backend::database>();
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
