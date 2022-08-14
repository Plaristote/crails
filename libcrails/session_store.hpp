#ifndef  SESSION_STORE_HPP
# define SESSION_STORE_HPP

# include "http.hpp"
# include "datatree.hpp"

# define USE_SESSION_STORE(classname) \
std::unique_ptr<Crails::SessionStore> Crails::SessionStore::Factory(void) \
{ \
  return (std::unique_ptr<Crails::SessionStore>(new classname ())); \
}

namespace Crails
{
  class BuildingResponse;

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
