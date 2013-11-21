#ifndef  MONGO_STORE_HPP
# define MONGO_STORE_HPP

# include "crails/cookie_data.hpp"
# include "crails/http_response.hpp"

# include <crails/mongodb/model.hpp>
# include <crails/mongodb/resultset.hpp>

class MongoStore
{
public:
  class SessionStore : public MongoDB::Model
  {
  public:
    static const std::string& CollectionName(void) { return (collection_name); }
    static const std::string& DatabaseName(void)   { return (database_name);   }

    MONGODB_MODEL(SessionStore)

    void SetFields(Data data);
    void GetFields(Data data);
    void Save(void);

    static void Cleanup(void);

    static const std::string  collection_name;
    static const std::string  database_name;
    static const unsigned int session_expiration;
  };

  MongoStore();

  void             Load(Data request_headers);
  void             Finalize(BuildingResponse& response);
  DynStruct&       Session(void)       { return (session_content); }
  const DynStruct& Session(void) const { return (session_content); }
  
private:
  SmartPointer<SessionStore> session;
  CookieData                 cookie;
  DynStruct                  session_content;
};

#endif