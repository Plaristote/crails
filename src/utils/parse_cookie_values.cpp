#include <crails/utils/parse_cookie_values.hpp>
#include <crails/http.hpp>
#include <Boots/Utils/regex.hpp>

using namespace std;

namespace Crails
{
  void parse_cookie_values(const string& str, std::function<bool (const std::string&, const std::string&)> callback)
  {
    string     cookie_string = str;
    Regex      regex;
    regmatch_t matches[3];

    regex.SetPattern("\\s*([^=]+)=([^;]*);{0,1}", REG_EXTENDED);
    while (!(regex.Match(cookie_string, matches, 3)))
    {
      string val     = cookie_string.substr(matches[2].rm_so, matches[2].rm_eo - matches[2].rm_so);
      string key     = cookie_string.substr(matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);

      cookie_string  = cookie_string.substr(matches[0].rm_eo);
      val            = Http::Url::Decode(val);
      key            = Http::Url::Decode(key);
      if (!(callback(key, val)))
        break ;
    }
  }
}
