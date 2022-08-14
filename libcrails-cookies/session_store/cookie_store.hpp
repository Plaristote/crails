#ifndef  COOKIE_STORE_HPP
# define COOKIE_STORE_HPP

# include "../cookie_data.hpp"
# ifdef CRAILS_B2_INCLUDE
# include "http_response.hpp"
# include "session_store.hpp"
# else
# include "../http_response.hpp"
# include "../session_store.hpp"
# endif

namespace Crails
{
  class CookieStore : public SessionStore
  {
  public:
    void             load(const HttpRequest&);
    void             finalize(BuildingResponse&);
    Data             to_data(void);
    const Data       to_data(void) const;

  private:
    CookieData       cookies;
  };
}

#endif
