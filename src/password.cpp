#include "crails/password.hpp"
#include <Boots/Utils/string.hpp>

using namespace std;
using namespace Crails;

string Password::md5(const string& str)
{
  unsigned char result[MD5_DIGEST_LENGTH];

  MD5((const unsigned char*)str.c_str(), str.size(), result);
  return (String::base64_encode(result, MD5_DIGEST_LENGTH));
}
