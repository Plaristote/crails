#include <crails/front/cookies.hpp>
#include <crails/http.hpp>
#include <crails/utils/parse_cookie_values.hpp>
#include <cheerp/client.h>
#include <cheerp/clientlib.h>

using namespace Crails::Front;
using namespace std;

Crails::Front::Cookies cookies;

bool Cookies::has(const string& key)
{
  ensure_cookie_map_is_updated();
  return cookie_store.find(key) != cookie_store.end();
}

template<>
string Cookies::get<string>(const string& key)
{
  ensure_cookie_map_is_updated();
  if (!has(key))
    client::eval(("throw 'std::runtime_error(\"cookie `" + key + "` does not exist\")'").c_str());
  return cookie_store.at(key);
}

template<>
void Cookies::set<string>(const string& key, const string& val)
{
  ensure_cookie_map_is_updated();
  if (has(key))
    cookie_store.at(key) = val;
  else
    cookie_store.insert(pair<string, string>(key, val));
  set_cookie_string(
    Http::Url::Encode(key) + '=' + Http::Url::Encode(val)
  );
}

void Cookies::remove(const string& key)
{
  ensure_cookie_map_is_updated();
  cookie_store.erase(key);
  set_cookie_string(
    Http::Url::Encode(key) + "=null; expires=Sat, 05 Nov 2016 00:00:00 GMT"
  );
  last_cookie_string = get_cookie_string();
}

void Cookies::update_cookie_map()
{
  last_cookie_string = get_cookie_string();
  Crails::parse_cookie_values(last_cookie_string,
    [this](const string& key, const string& value) {
    cookie_store.emplace(key, value);
    return true;
  });
}

string Cookies::get_cookie_string() const
{
  return (string)(*client::document.get_cookie());
}

void Cookies::set_cookie_string(const string& str) const
{
  client::document.set_cookie(str.c_str());
}
