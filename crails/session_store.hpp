#ifndef  SESSION_STORE_HPP
# define SESSION_STORE_HPP

# include "crails/cookie_data.hpp"
# include "crails/http_response.hpp"

# define USE_SESSION_STORE(classname) \
std::unique_ptr<SessionStore> SessionStore::Factory(void) \
{ \
  return (std::unique_ptr<SessionStore>(new classname ())); \
}

namespace Crails
{
  class SessionStore
  {
  public:
    virtual ~SessionStore() {}
    static std::unique_ptr<SessionStore> Factory(void);

    virtual void load(const HttpRequest&)    = 0;
    virtual void finalize(BuildingResponse&) = 0;
    virtual Data to_data(void)               = 0;
  };
}

#endif
