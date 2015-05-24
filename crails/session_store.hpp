#ifndef  SESSION_STORE_HPP
# define SESSION_STORE_HPP

# include "crails/cookie_data.hpp"
# include "crails/http_response.hpp"
# include <Boots/Utils/smart_pointer.hpp>

# define USE_SESSION_STORE(classname) \
SmartPointer<SessionStore> SessionStore::Factory(void) \
{ \
  return (new classname ()); \
}

namespace Crails
{
  class SessionStore
  {
  public:
    virtual ~SessionStore() {}
    static SmartPointer<SessionStore> Factory(void);

    virtual void             load(Data request_headers)           = 0;
    virtual void             finalize(BuildingResponse& response) = 0;
    virtual DynStruct&       to_data(void)                        = 0;
    virtual const DynStruct& to_data(void) const                  = 0;
  };
}

#endif
