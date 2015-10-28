#include "crails/session_store/cookie_store.hpp"

using namespace std;
using namespace Crails;

void CookieStore::load(Data request_headers)
{
  string cookie_string = request_headers["Cookie"].defaults_to<string>("");

  cookies.unserialize(cookie_string);
}

void CookieStore::finalize(BuildingResponse& response)
{
  if (cookies.as_data().count() > 0)
    response.set_headers("Set-Cookie", cookies.serialize());
}

Data CookieStore::to_data(void)
{
  return (cookies.as_data());
}
