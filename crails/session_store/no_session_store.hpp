#ifndef  NO_SESSION_STORE
# define NO_SESSION_STORE

# include <crails/session_store.hpp>
# include <crails/datatree.hpp>

namespace Crails
{
  class NoSessionStore : public SessionStore
  {
  public:
    void load(Data) {}
    void finalize(BuildingResponse&) {}
    void Data to_data(void) { return stub.to_data(); }
  private:
    DataTree stub;
  };
}

#endif