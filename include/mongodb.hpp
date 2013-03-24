#ifndef  MY_MONGODB_HPP
# define MY_MONGODB_HPP

# include "crails/databases.hpp"
# include "crails/mongodb/database.hpp"
# include <iostream>

class MongodbDb : public Databases::Db, public MongoDB::Database
{
public:
  static const std::string ClassType(void) { return ("mongodb"); }

  MongodbDb(Data data) : Db(ClassType()),
                         Database(connection, data["database"].Value()),
                         hostname("127.0.0.1"),
                         port(27017)
  {
    Data conf_hostname = data["hostname"];
    Data conf_port     = data["port"];
    Data conf_username = data["username"];
    Data conf_password = data["password"];
    
    if (conf_hostname.NotNil())
      hostname = conf_hostname.Value();
    if (conf_port.NotNil())
      port     = conf_port;
    if (conf_username.NotNil())
      username = conf_username.Value();
    if (conf_password.NotNil())
      password = conf_password.Value();
    connected = false;
  }
  
  void Connect(void)
  {
    if (!connected)
    {
      std::stringstream stream;
      std::string       err;
      
      stream << hostname << ':' << port;
      if (connection.connect(stream.str(), err))
      {
        std::cout << "[MongoDB] New connection to " << stream.str() << std::endl;
        RefreshCollections();
        connected = true;
      }
      else
        throw Databases::Exception(err);
    }
  }

private:
  mongo::DBClientConnection connection;
  std::string               hostname;
  unsigned short            port;
  std::string               username;
  std::string               password;
  bool                      connected;
};

#endif
