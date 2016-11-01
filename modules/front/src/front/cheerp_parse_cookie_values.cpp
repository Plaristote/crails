#include <crails/utils/parse_cookie_values.hpp>
#include <crails/http.hpp>
#include <regex>
#include <iostream>

using namespace std;

static bool trigger_add_cookie_value(const string& str, std::function<bool (const string&, const string&)> callback, size_t key_start, size_t key_end, size_t val_start, size_t val_end)
{
  string key = str.substr(key_start, key_end - key_start);
  string val = str.substr(val_start, val_end - val_start);
  key = Crails::Http::Url::Decode(key);
  val = Crails::Http::Url::Decode(val);
  return callback(key, val);
}

namespace Crails
{
  void parse_cookie_values(const string& str, std::function<bool (const std::string&, const std::string&)> callback)
  {
    char   state = 0;
    size_t key_start = 0;
    size_t key_end = 0;
    size_t val_start = 0;

    for (size_t i = 0 ; i < str.size() ; ++i)
    {
      switch (state)
      {
        case 0: // spaces
          if (str[i] == ' ') continue ;
          key_start = i;
          state++;
        case 1: // key
          if (str[i] == '=')
          {
            key_end = i;
            val_start = i + 1;
            state++;
          }
          break ;
        case 2: // value
          if (str[i] == ';')
          {
            state = 0;
            if (!(trigger_add_cookie_value(str, callback,
               key_start, key_end, val_start, i)))
              return ;
          }
      }
    }
    if (state == 2)
    {
      trigger_add_cookie_value(str, callback,
        key_start, key_end, val_start, str.size());
    }
  }
}
