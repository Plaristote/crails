#include <crails/utils/parse_cookie_values.hpp>
#include <crails/url.hpp>
#include <regex>

using namespace std;

namespace Crails
{
  void parse_cookie_values(const string& str, std::function<bool (const std::string&, const std::string&)> callback)
  {
    regex regexp("\\s*([^=]+)=([^;]*);{0,1}", regex_constants::ECMAScript);
    auto  matches = sregex_iterator(str.begin(), str.end(), regexp);

    for (auto it = matches ; it != sregex_iterator() ; ++it)
    {
      smatch match = *it;
      string val = str.substr(match.position(2), match.length(2));
      string key = str.substr(match.position(1), match.length(1));

      val = Url::decode(val);
      key = Url::decode(key);
      if (!(callback(key, val)))
        break ;
    }
  }
}
