#include "crails/session_store/cookie_store.hpp"

using namespace std;
using namespace Crails;

void CookieStore::load(Data request_headers)
{
  string     cookie_string = request_headers["Cookie"].Value();

  cookies.Unserialize(cookie_string);
}

void CookieStore::finalize(BuildingResponse& response)
{
  if (cookies.Count() > 0)
    response.SetHeaders("Set-Cookie", cookies.Serialize());
}

DynStruct& CookieStore::to_data(void)
{
  return (cookies);
}

const DynStruct& CookieStore::to_data(void) const
{
  return (cookies);
}
