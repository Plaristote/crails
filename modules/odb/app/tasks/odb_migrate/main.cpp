#include <crails/odb/database.hpp>
#include <odb/schema-catalog.hxx>

using namespace std;

int help(char* arg_command)
{
  cout << "Usage: " << arg_command << " [options] database_key" << endl;
  cout << "Options:" << endl;
  cout << "\t-c: create the database before performing the migration" << endl;
  cout << "\t-d: drop the schema instead of performing a migration" << endl;
  return -1;
}

void create_database(const std::string& database_name)
{
  const auto& settings = Crails::Databases::settings.at(Crails::environment).at(database_name);

  // The second and third parameters are the user and password for the database,
  // If left empty, it uses the identity specified in the database settings.
  // Make sure to use the credentials of a user which can create users and databases.
  ODB::Database::create_from_settings(settings, "", "");
}

int main(int argc, char** argv)
{
  if (argc < 2)
    return help(argv[0]);
  else
  {
    string database_name = argv[argc - 1];
    string option        = argc > 2 ? string(argv[1]) : string("");

    if (option == "-c")
      create_database(database_name);
    {
      ODB::Database& database = CRAILS_DATABASE(ODB, database_name);

      if (option == "-d")
        database.drop();
      else
        database.migrate();
    }
  }
  return 0;
}
