#include <vector>
#include <Boots/Utils/dynstruct.hpp>
#include <Boots/Utils/regex.hpp>
#include "crails/http.hpp"

using namespace std;

static void RecursivelySetValue(Data param, std::vector<std::string> key_stack, const std::string& value)
{
  if (key_stack.size() == 0)
    param = value;
  else
  {
    Data next = param[key_stack.front()];

    key_stack.erase(key_stack.begin());
    RecursivelySetValue(next, key_stack, value);
  }
}

void cgi2params(DynStruct& params, const std::string& encoded_str)
{
  std::string    str        = Http::Url::Decode(encoded_str);  
  std::string    looping    = "";
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
      key_stacks.push_back(str.substr(match.rm_so, match.rm_eo));
      str.erase(match.rm_so, match.rm_eo);

      while (str[0] == '[')
      {
        str.erase(0, 1);
        if (!(regex.Match(str, &match, 1)))
        {
          key_stacks.push_back(str.substr(match.rm_so, match.rm_eo));
          str.erase(match.rm_so, match.rm_eo);
          if (str[0] == ']')
            str.erase(0, 1);
          else
            return ;
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