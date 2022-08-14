#ifndef  NO_SESSION_STORE
# define NO_SESSION_STORE

# include "../session_store.hpp"
# include "../datatree.hpp"

namespace Crails
{
  class NoSessionStore : public SessionStore
  {
  public:
    void load(const HttpRequest&) {}
    void finalize(BuildingResponse&) {}
    Data to_data() { return stub.as_data(); }
  private:
    DataTree stub;
  };
}

#endif
