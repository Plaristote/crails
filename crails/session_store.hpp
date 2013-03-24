#ifndef  SESSION_STORE_HPP
# define SESSION_STORE_HPP

# include "crails/cookie_data.hpp"
# include "crails/http_response.hpp"

/*
 * Cookie based SessionStore
 */
class CookieStore
{
public:
  void             Load(Data request_headers);
  void             Finalize(BuildingResponse& response);
  DynStruct&       Session(void);
  const DynStruct& Session(void) const;

private:
  CookieData       cookies;
};

/*
 * MongoDB based SesionStore
 */
/*# include <mongo/client/dbclient.h>

class MongoStore
{
public:
  MongoStore(Data request_headers);
  
  DynStruct&       Session(void);
  const DynStruct& Session(void) const;
  
  void             Finalize(BuildingResponse& response);
private:
  CookieData cookie;
  DynStruct  variables;
  mongo::DBClientConnection connection;
};*/

typedef CookieStore SessionStore;

#endif