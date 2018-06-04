#include <crails/utils/string.hpp>

using namespace std;

namespace Crails
{
  list<string> split(const std::string& str, char c, bool count_repetitions)
  {
    list<string>   ret;
    int            last_sep = -1;
    unsigned int   i;

    for (i = 0 ; i < str.size() ; ++i)
    {
      if (str[i] == c)
      {
        if (i != 0 && (count_repetitions || str[i - 1] != c))
          ret.push_back(str.substr(last_sep + 1, i - (last_sep + 1)));
        last_sep = i;
      }
    }
    if ((unsigned int)last_sep != i && i > 0 && str[i - 1] != c)
      ret.push_back(str.substr(last_sep + 1));
    return (ret);
  }
}
