#include <crails/cookie_data.hpp>
#include <crails/http.hpp>
#include <crails/cipher.h>
#include <crails/utils/parse_cookie_values.hpp>
#include <crails/utils/regex.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace Crails;
using namespace boost::property_tree;

string CookieData::serialize(void)
{
  try
  {
    Cipher cipher;
    string cookie_string;
    string value   = this->as_data().to_json();

    if (use_encryption)
      value        = cipher.encrypt(value, password, salt);
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
    parse_cookie_values(str, [this](const string& key, const string& val) -> bool
    {
      if (key == "crails")
      {
        if (use_encryption)
        {
          Cipher cipher;
          this->from_json(cipher.decrypt(val, password, salt));
        }
        else
          this->from_json(val);
        return false;
      }
      return true;
    });
  }
  catch (const std::exception&)
  {
  }
}
