#include <vector>
#include <Boots/Utils/dynstruct.hpp>
#include <Boots/Utils/regex.hpp>
#include "crails/http.hpp"

using namespace std;

static void RecursivelySetValue(Data param, std::vector<std::string> key_stack, const std::string& value)
{
  if (key_stack.size() == 0)
    param = Http::Url::Decode(value);
  else
  {
    Data next = param[Http::Url::Decode(key_stack.front())];

    key_stack.erase(key_stack.begin());
    RecursivelySetValue(next, key_stack, value);
  }
}

void cgi2params(DynStruct& params, const std::string& encoded_str)
{
  const std::string        opening_bracket = "%5B";
  const std::string        closing_bracket = "%5D";
  std::string              str             = encoded_str;
  std::string              looping         = "";
  std::vector<std::string> key_stacks;

  while (str.size())
  {
    Regex      regex;
    regmatch_t match;

    if (str == looping)
      return ;
    looping = str;
    regex.SetPattern("[^=&]*", REG_EXTENDED);
    if (!(regex.Match(str, &match, 1)))
    {
      auto sub_key = str.find(opening_bracket);

      if (sub_key != std::string::npos && (int)sub_key < match.rm_eo)
        match.rm_eo = sub_key;
      key_stacks.push_back(str.substr(match.rm_so, match.rm_eo));
      str.erase(match.rm_so, match.rm_eo);

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
        if (!(regex.Match(str, &match, 1)))
        {
          RecursivelySetValue(params, key_stacks, str.substr(match.rm_so, match.rm_eo));
          key_stacks.clear();
          str.erase(match.rm_so, match.rm_eo);
        }
      }
      if (str[0] == '&')
        str.erase(0, 1);
    }
    else
      return ;
  }
}
