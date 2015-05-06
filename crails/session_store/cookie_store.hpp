#ifndef  COOKIE_STORE_HPP
# define COOKIE_STORE_HPP

# include "crails/cookie_data.hpp"
# include "crails/http_response.hpp"
# include "crails/session_store.hpp"

namespace Crails
{
  class CookieStore : public SessionStore
  {
  public:
    void             Load(Data request_headers);
    void             Finalize(BuildingResponse& response);
    DynStruct&       Session(void);
    const DynStruct& Session(void) const;

  private:
    CookieData       cookies;
  };
}

#endif