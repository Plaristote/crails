#ifndef  MONGO_STORE_HPP
# define MONGO_STORE_HPP

# include "crails/cookie_data.hpp"
# include "crails/http_response.hpp"
# include "crails/session_store.hpp"

# include <crails/mongodb/model.hpp>
# include <crails/mongodb/resultset.hpp>

namespace Crails
{
  class MongoStore : public SessionStore
  {
  public:
    class SessionStore : public MongoDB::Model
    {
    public:
      static const std::string& CollectionName(void) { return (collection_name); }
      static const std::string& DatabaseName(void)   { return (database_name);   }

      MONGODB_MODEL(SessionStore)

      void set_fields(Data data);
      void get_fields(Data data);
      void save(void);

      static void Cleanup(void);

      static const std::string  collection_name;
      static const std::string  database_name;
      static const unsigned int session_expiration;
    };

    MongoStore();
    ~MongoStore();

    void             load(Data request_headers);
    void             finalize(BuildingResponse& response);
    DynStruct&       to_data(void)       { return (session_content); }
    const DynStruct& to_data(void) const { return (session_content); }
    
  private:
    SmartPointer<SessionStore> session;
    CookieData                 cookie;
    DynStruct                  session_content;
  };
}

#endif
