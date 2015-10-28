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
    void             load(Data request_headers);
    void             finalize(BuildingResponse& response);
    Data             to_data(void);
    const Data       to_data(void) const;

  private:
    CookieData       cookies;
  };
}

#endif
