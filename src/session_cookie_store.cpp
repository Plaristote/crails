#include "crails/session_store/cookie_store.hpp"

using namespace std;
using namespace Crails;

void CookieStore::Load(Data request_headers)
{
  string     cookie_string = request_headers["Cookie"].Value();

  cookies.Unserialize(cookie_string);
}

void CookieStore::Finalize(BuildingResponse& response)
{
  if (cookies.Count() > 0)
    response.SetHeaders("Set-Cookie", cookies.Serialize());
}

DynStruct& CookieStore::Session(void)
{
  return (cookies);
}

const DynStruct& CookieStore::Session(void) const
{
  return (cookies);
}
