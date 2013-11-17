#include "crails/session_store.hpp"

using namespace std;

void CookieStore::Load(Data request_headers)
{
  string     cookie_string = request_headers["Cookie"].Value();

  cookies.Unserialize(cookie_string);
}

void CookieStore::Finalize(BuildingResponse& response)
{
  if (cookies.Count() > 0)
    response.SetHeaders("Set-Cookie", cookies.Serialize());
  //else
  //  response.SetHeaders("Set-Cookie", "expires=Thu, 01 Jan 1970 00:00:00 GMT");
}

DynStruct& CookieStore::Session(void)
{
  return (cookies);
}

const DynStruct& CookieStore::Session(void) const
{
  return (cookies);
}