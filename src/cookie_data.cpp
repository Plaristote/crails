#include <crails/cookie_data.hpp>
#include <crails/http.hpp>
#include <crails/cipher.h>
#include <Boots/Utils/regex.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace Crails;
using namespace boost::property_tree;

static std::string charToHex(char c)
{
  std::string result;
  char first, second;

  first = (c & 0xF0) / 16;
  first += first > 9 ? 'A' - 10 : '0';
  second = c & 0x0F;
  second += second > 9 ? 'A' - 10 : '0';

  result.append(1, first);
  result.append(1, second);

  return result;
}

static char hexToChar(char first, char second)
{
  int digit;

  digit = (first >= 'A' ? ((first & 0xDF) - 'A') + 10 : (first - '0'));
  digit *= 16;
  digit += (second >= 'A' ? ((second & 0xDF) - 'A') + 10 : (second - '0'));
  return static_cast<char>(digit);
}

std::string Http::Url::Encode(const std::string& src)
{
  std::string                 result;
  std::string::const_iterator iter;

  for(iter = src.begin() ; iter != src.end() ; ++iter)
  {
    switch(*iter)
    {
    case ' ':
      result.append(1, '+');
      break;
      // alnum
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
    case 'V': case 'W': case 'X': case 'Y': case 'Z':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
    case 'v': case 'w': case 'x': case 'y': case 'z':
    case '0': case '1': case '2': case '3': case '4': case '5': case '6':
    case '7': case '8': case '9':
      // mark
    case '-': case '_': case '.': case '!': case '~': case '*': case '\'': 
    case '(': case ')':
      result.append(1, *iter);
      break;
      // escape
    default:
      result.append(1, '%');
      result.append(charToHex(*iter));
      break;
    }
  }  
  return (result);
}

std::string Http::Url::Decode(const std::string& src)
{
  std::string                 result;
  std::string::const_iterator iter;
  char                        c;

  for(iter = src.begin(); iter != src.end(); ++iter)
  {
    switch(*iter)
    {
    case '+':
      result.append(1, ' ');
      break;
    case '%':
      // Don't assume well-formed input
      if(std::distance(iter, src.end()) >= 2
         && std::isxdigit(*(iter + 1)) && std::isxdigit(*(iter + 2))) {
        c = *++iter;
        result.append(1, hexToChar(c, *++iter));
      }
      // Just pass the % through untouched
      else {
        result.append(1, '%');
      }
      break;

    default:
      result.append(1, *iter);
      break;
    }
  }
  return (result);
}

string CookieData::serialize(void)
{
  try
  {
    Cipher cipher;
    string cookie_string;
    string value   = this->as_data().to_json();
 
    value          = cipher.encrypt(value, password, salt);
    cookie_string += Http::Url::Encode("crails") + '=' + Http::Url::Encode(value);
    cookie_string += ";path=/";
    return (cookie_string);
  }
  catch (const std::exception&)
  {
  }
  return ("");
}

void CookieData::unserialize(const string& str)
{
  try
  {
    Cipher     cipher;
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
      if (key == "crails")
      {
        val = cipher.decrypt(val, password, salt);
        {
          this->from_json(val);
          break ;
        }
      }
    }
  }
  catch (const std::exception&)
  {
  }
}
