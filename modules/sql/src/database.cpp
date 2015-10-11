#include "crails/sql/database.hpp"
#include "crails/sql/table.hpp"

using namespace std;
using namespace SQL;
using namespace Crails;

/*
 * Database
 */
Database::Database(Data settings) : Db(ClassType()), connected(false)
{
  string backend = settings["type"].Value();

  if (backend == "mysql")
    InitializeForMySQL();
  else if (backend == "postgresql")
    InitializeForPostgreSQL();
  else
    InitializeForSqlite();
  SetDbName(settings["database"].Value());
}

void Database::InitializeForMySQL(Data data)
{
  string cmd = "dbname=" + data["database"].Value();

  if (data["user"].NotNil())
    cmd += " user=" + data["user"].Value();
  if (data["password"].NotNil())
    cmd += " password=" + data["password"].Value();
  if (data["host"].NotNil())
    cmd += " host=" + data["host"].Value();
  if (data["port"].NotNil())
    cmd += " port=" + data["port"].Value();
  if (data["unix_socket"].NotNil())
    cmd += " unix_socket=" + data["unix_socket"].Value();
  connect_cmd = cmd;
  factory     = "mysql";
}

void Database::InitializeForPostgreSQL(Data data)
{
  factory     = "postgresql";
  connect_cmd = "dbname=" + data["database"].Value();
}

void Database::InitializeForSqlite(Data data)
{
  string cmd = "sqlite.db";

  if (data["file"].NotNil())
    cmd = data["file"].Value();
  connect_cmd = cmd;
  factory     = "sqlite3";
}

Table Database::operator[](const std::string& name)
{
  if (!(TableExists(name)))
    throw Databases::Exception("SQL Table " + name + " does not exist");
  return (Table(sql, name));
}

TableDescription Database::Describe(const std::string& table_name)
{
  return (TableDescription(sql, table_name));
}

bool Database::TableExists(const std::string& table_name)
{
  std::stringstream query;
  int               count;

  query << "select count(*) from INFORMATION_SCHEMA.TABLES where";
  query << " TABLE_TYPE='BASE TABLE' and TABLE_NAME='" << table_name << '\'';
  query << " and TABLE_SCHEMA='" << name << "'";
  cout << query.str() << endl;
  sql << query.str(), soci::into(count);
  return (count != 0);
}

/*
 * Table Description
 */
TableDescription::TableDescription(soci::session& sql, const std::string name) : table_name(name), sql(sql)
{
  try
  {
    soci::rowset<soci::row> rs  = (sql.prepare << "describe " << name);
    auto                    it  = rs.begin();
    auto                    end = rs.end();

    for (; it != end ; ++it)
    {
      soci::row&  row = *it;
      Field       field;
      
      field.name = row.get<std::string>(0);
      field.type = row.get<std::string>(1);
      fields.push_back(field);
    }
    table_exists = true;
  }
  catch (const std::exception& e)
  {
    table_exists = false;
  }
}

void TableDescription::SetTableSchema(std::vector<Field> updated_fields)
{
  table_exists ? UpdateTable(updated_fields) : InsertTable(updated_fields);
}

void TableDescription::InsertTable(std::vector<Field> updated_fields)
{
  unsigned int i = 0;
  std::stringstream query;

  query << "create table " << table_name << " (";
  for (; i < updated_fields.size(); ++i)
  {
    query << updated_fields[i].name << ' ' << updated_fields[i].type;
    if (i + 1 < updated_fields.size())
      query << ',';
  }
  query << ')';
  cout << query.str() << endl;
  sql << query.str();
}

void TableDescription::UpdateTable(std::vector<Field> updated_fields)
{
  unsigned int i = 0;
  
  /*
    * Move or erase fields that are moved or discarded in updated_fields
    */
  while (i < fields.size() && i < updated_fields.size())
  {
    cout << "Type is " << updated_fields[i].type << endl;
    if (updated_fields[i].name == fields[i].name)
    {
      if (updated_fields[i].type != fields[i].type && updated_fields[i].name != "id")
      {
        std::stringstream query;
        
        query << "alter table " << table_name << " change column ";
        query << fields[i].name << ' ' << fields[i].name << ' ' << updated_fields[i].type;
        cout << query.str() << endl;
        sql << query.str();
      }
      ++i;
    }
    else
    {
      unsigned int      ii;
      std::stringstream query;
      
      query << "alter table " << table_name << ' ';
      for (ii = i ; ii < fields.size() ; ++ii)
      {
        if (fields[ii].name == updated_fields[i].name)
        {
          Field swap = fields[i];
          
          fields[i]  = fields[ii];
          fields[ii] = swap;
          query << "modify column ";
          if (i > 0)
            query << fields[i].name << ' ' << updated_fields[i].type << " after " << fields[i - 1].name;
          else
            query << fields[i].name << ' ' << updated_fields[i].type << " first";
          break ;
        }
      }
      if (ii != fields.size())
        ++i;
      else
      {
        query << "add column " << updated_fields[i].name << ' ' << updated_fields[i].type << ' ';
        if (i > 0)
          query << " after " << fields[i - 1].name;
        else
          query << " first";
        auto it = fields.begin();
        std::advance(it, i);
        fields.insert(it, updated_fields[i]);
      }
      cout << query.str() << endl;
      sql << query.str();
    }
  }
  /*
    * Drop fields that were discarded
    */
  while (i < fields.size() && i >= updated_fields.size())
  {
    std::stringstream query;

    query << "alter table " << table_name << " drop column " << fields[i].name;
    cout << query.str() << endl;
    sql << query.str();
    auto it = fields.begin();
    std::advance(it, i);
    fields.erase(it);
  }
  /*
    * Add new fields
    */
  while (i < updated_fields.size())
  {
    std::stringstream query;
    
    query << "alter table " << table_name << " add column " << updated_fields[i].name << ' ' << updated_fields[i].type;
    cout << query.str() << endl;
    sql << query.str();
    fields.push_back(updated_fields[i]);
    i++;
  }
}
