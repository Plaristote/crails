#include <crails/utils/string.hpp>

using namespace std;

namespace Crails
{
  string strip(const string& str)
  {
    unsigned short w_begin, w_end;
    bool           got_begin = false;

    for (unsigned short i = 0 ; i < str.size() ; ++i)
    {
      if (str[i] != ' ')
      {
        if (got_begin == false)
        {
          w_begin   = i;
          got_begin = true;
        }
        w_end = i;
      }
    }
    return (str.substr(w_begin, (w_end + 1) - w_begin));
  }

  string humanize(const string& tmp)
  {
    string str = strip(tmp);
    string ret;

    for (unsigned short i = 0 ;  i < str.size() ; ++i)
    {
      if (i == 0 || str[i - 1] == '_' || str[i - 1] == ' ')
      {
        if (str[i] >= 'a' && str[i] <= 'z')
          ret += str[i] - 'a' + 'A';
        else
          ret += str[i];
      }
      else if (str[i] == '_')
      {
        if (i == 0 || str[i - 1] != '_')
          ret += ' ';
      }
      else
        ret += str[i];
    }
    return (ret);
  }

  string underscore(const string& tmp)
  {
    string         str = strip(tmp);
    string         ret;

    for (unsigned short i = 0 ;  i < str.size() ; ++i)
    {
      if      (str[i] >= 'A' && str[i] <= 'Z')
        ret += str[i] - 'A' + 'a';
      else if (str[i] == ' ')
      {
        if (str[i - 1] != ' ')
          ret += '_';
      }
      else
        ret += str[i];
    }
    return (ret);
  }

  string singularize(const string& tmp)
  {
    if (tmp.length() > 3)
    {
      string suffix = tmp.substr(tmp.length() - 3);

      if (suffix == "ies")
        return tmp.substr(0, tmp.length() - 3) + 'y';
      else if (suffix[1] == 'e' && suffix[2] == 's')
      {
        if (suffix[0] == 's')
          return tmp.substr(0, tmp.length() - 2);
        return tmp.substr(0, tmp.length() - 1);
      }
    }
    return tmp.substr(0, tmp.length() - 1);
  }

  string pluralize(const string& tmp)
  {
    if (tmp.length() > 2)
    {
      string suffix = tmp.substr(tmp.length() - 2);

      if (suffix[1] == 's' || suffix[1] == 'x' || suffix[1] == 'z' || suffix == "ch" || suffix == "sh")
        return tmp + "es";
      else if (suffix[1] == 'y')
        return tmp.substr(0, tmp.length() - 1) + "ies";
    }
    return tmp + 's';
  }
}
