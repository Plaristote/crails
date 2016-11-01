#include <vector>
#include <regex>
#include "crails/http.hpp"
#include "crails/datatree.hpp"

using namespace std;
using namespace Crails;

static void recursively_set_value(Data param, std::vector<std::string> key_stack, const std::string& value)
{
  if (key_stack.size() == 0)
    param = Http::Url::Decode(value);
  else
  {
    string key = Http::Url::Decode(key_stack.front());

    key_stack.erase(key_stack.begin());
    if (key.length() == 0)
    {
      DataTree sub_object;

      recursively_set_value(sub_object.as_data(), key_stack, value);
      param.push_back(sub_object.as_data());
    }
    else
    {
      Data next = param[key];

      recursively_set_value(next, key_stack, value);
    }
  }
}

void cgi2params(Data params, const std::string& encoded_str)
{
  const std::string        opening_bracket = "%5B";
  const std::string        closing_bracket = "%5D";
  std::string              str             = encoded_str;
  std::string              looping         = "";
  std::vector<std::string> key_stacks;

  while (str.size())
  {
    if (str == looping)
      return ;
    regex regexp("[^=&]*", regex_constants::ECMAScript);
    auto  matches = sregex_iterator(str.begin(), str.end(), regexp);

    looping = str;
    if (distance(matches, sregex_iterator()) > 0)
    {
      auto sub_key = str.find(opening_bracket);
      smatch match = *matches;
      auto   eo    = match.length(0);

      if (sub_key != std::string::npos && (int)sub_key < match.position(0) + eo)
        eo = sub_key - match.position(0);
      key_stacks.push_back(str.substr(match.position(0), eo));
      str.erase(match.position(0), eo);

      while (str.find(opening_bracket) == 0)
      {
        str.erase(0, opening_bracket.size());
        auto end_key = str.find(closing_bracket);
        if (end_key != std::string::npos)
        {
          key_stacks.push_back(str.substr(0, end_key));
          str.erase(0, end_key + closing_bracket.size());
        }
        else
          return ;
      }
      if (str[0] == '=')
      {
        str.erase(0, 1);
        matches = sregex_iterator(str.begin(), str.end(), regexp);
        if (distance(matches, sregex_iterator()) > 0)
        {
          match = *matches;
          recursively_set_value(params, key_stacks, str.substr(match.position(0), match.length(0)));
          key_stacks.clear();
          str.erase(match.position(0), match.length(0));
        }
      }
      if (str[0] == '&')
        str.erase(0, 1);
    }
    else
      return ;
  }
}
