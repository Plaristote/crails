#include <crails/utils/string.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

using namespace Crails;
using namespace std;

namespace Crails
{
  string generate_random_string(const string& charset, unsigned short length)
  {
    string result;
    boost::random::random_device rng;
    boost::random::uniform_int_distribution<> index_dist(0, charset.size() - 1);

    result.reserve(length);
    for (unsigned short i = 0 ; i < length ; ++i)
      result += charset[index_dist(rng)];
    return result;
  }

  list<string> split(const std::string& str, char c, bool count_repetitions)
  {
    list<string> ret;
    short        last_sep = -1;
    short        i;

    for (i = 0 ; str[i] ; ++i)
    {
      if (str[i] == c)
      {
        if (i != 0 && (count_repetitions || str[i - 1] != c))
          ret.push_back(str.substr(last_sep + 1, i - (last_sep + 1)));
        last_sep = i;
      }
    }
    if (last_sep != i && i > 0 && str[i - 1] != c)
      ret.push_back(str.substr(last_sep + 1));
    return (ret);
  }

  string str_strip(const string& str)
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
    string str = str_strip(tmp);
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
    string         str = str_strip(tmp);
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
          return tmp.substr(0, tmp.length() - 1);
        return tmp.substr(0, tmp.length() - 2);
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

  string base64_encode(string str_to_encode)
  {
    return (base64_encode((unsigned char const*)str_to_encode.c_str(), str_to_encode.size()));
  }
  
  string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len)
  {
    static const string  charset         =  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                            "abcdefghijklmnopqrstuvwxyz"
                                            "0123456789+/";
    string        ret;
    int           i = 0;
    int           j = 0;
    unsigned char array_3[3];
    unsigned char array_4[4];

    while (in_len--) {
      array_3[i++] = *(bytes_to_encode++);
      if (i == 3) {
        array_4[0] = (array_3[0] & 0xfc) >> 2;
        array_4[1] = ((array_3[0] & 0x03) << 4) + ((array_3[1] & 0xf0) >> 4);
        array_4[2] = ((array_3[1] & 0x0f) << 2) + ((array_3[2] & 0xc0) >> 6);
        array_4[3] = array_3[2] & 0x3f;

        for(i = 0; (i <4) ; i++)
          ret += charset[array_4[i]];
        i = 0;
      }
    }

    if (i)
    {
      for(j = i; j < 3; j++)
        array_3[j] = '\0';
      array_4[0] = (array_3[0] & 0xfc) >> 2;
      array_4[1] = ((array_3[0] & 0x03) << 4) + ((array_3[1] & 0xf0) >> 4);
      array_4[2] = ((array_3[1] & 0x0f) << 2) + ((array_3[2] & 0xc0) >> 6);
      array_4[3] = array_3[2] & 0x3f;
      for (j = 0; (j < i + 1); j++)
        ret += charset[array_4[j]];
      while((i++ < 3))
        ret += '=';
    }
    return (ret);
  }
}
