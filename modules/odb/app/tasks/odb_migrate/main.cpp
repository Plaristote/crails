#include <boost/filesystem.hpp>
#include <cstdlib>
#include <iostream>
#include <crails/datatree.hpp>
#include <crails/environment.hpp>

using namespace std;
using namespace boost;

struct recursive_directory_range
{
  typedef boost::filesystem::recursive_directory_iterator iterator;
  recursive_directory_range(boost::filesystem::path p) : p(p) {}

  iterator begin() { return boost::filesystem::recursive_directory_iterator(p); }
  iterator end() { return boost::filesystem::recursive_directory_iterator(); }

  boost::filesystem::path p;
};

int run_sql_file(string command, const string& filepath)
{
  command += " -f \"" + filepath + '"';
  cout << "+ " << command << endl;
  return std::system(command.c_str());
}

int odb_migrate(const std::string& database_key, const vector<string>& sql_files)
{
  DataTree         config;
  Data             database_config = config[Crails::environment][database_key];
  string           command("psql");

  config.from_json_file("config/db.json");
  command += " -h " + database_config["host"].as<string>();
  command += " -p " + database_config["port"].defaults_to<string>("5432");
  command += " -d " + database_config["name"].as<string>();
  command += " -U " + database_config["user"].as<string>();
  command  = "PGPASSWORD=\"" + database_config["password"].as<string>() + "\" " + command;
  for (auto it : recursive_directory_range("./app/models"))
  {
    filesystem::path filepath = it.path();
    bool should_run_file = filepath.extension().string() == ".sql";

    should_run_file = should_run_file && (sql_files.size() == 0 || find(sql_files.begin(), sql_files.end(), filepath.string()) != sql_files.end());
    if (should_run_file)
    {
      int code = run_sql_file(command, filepath.string());

      if (code != 0)
        return code;
    }
  }
  return 0;
}

int main(int argc, char** argv)
{
  if (argc < 2)
    cout << "Usage: crails task run odb_migrate [database_key]" << endl;
  else
  {
    vector<string> sql_files;

    for (int i = 2 ; i < argc ; ++i)
      sql_files.push_back(string("./app/models/") + argv[i] + ".sql");
    return odb_migrate(argv[1], sql_files);
  }
  return 1;
}
