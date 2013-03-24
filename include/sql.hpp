#ifndef  MY_SQL_HPP
# define MY_SQL_HPP

# include "crails/databases.hpp"
# include "crails/sql/database.hpp"

class SqlDb : public Databases::Db, public SQL::Database
{
public:
  static const std::string ClassType(void) { return ("sql"); }

  SqlDb(Data data) : Db(ClassType()),
                     Database(soci::mysql, DbInitLine(data))
  {
    SetDbName(data["database"].Value());
    connected = false;
  }

  void Connect(void)
  {
    if (!connected)
    {
      OpenConnection();
      connected = true;
    }
  }

private:
  static const std::string DbInitLine(Data data)
  {
    std::string cmd = "dbname=" + data["database"].Value();
    
    if (data["user"].NotNil())
      cmd += " user=" + data["user"].Value();
    if (data["password"].NotNil())
      cmd += " password=" + data["password"].Value();
    return (cmd);
  }
  
  bool connected;
};

#endif