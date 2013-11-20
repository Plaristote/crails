#ifndef  COOKIE_STORE_HPP
# define COOKIE_STORE_HPP

# include "crails/cookie_data.hpp"
# include "crails/http_response.hpp"

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

#endif