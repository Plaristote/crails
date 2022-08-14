#include "cookie_data.hpp"
#include "url.hpp"
#include "utils/parse_cookie_values.hpp"
#include <boost/property_tree/json_parser.hpp>
#include "cipher.hpp"

using namespace std;
using namespace Crails;
using namespace boost::property_tree;

string CookieData::serialize(void)
{
  try
  {
    string cookie_string;
    string value   = this->as_data().to_json();
    Cipher cipher;

    if (use_encryption)
      value        = cipher.encrypt(value, password, salt);
    cookie_string += Url::encode("crails") + '=' + Url::encode(value);
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
