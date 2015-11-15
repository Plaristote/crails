#include "crails/password.hpp"
#include "crails/utils/string.hpp"

using namespace std;
using namespace Crails;

string Password::md5(const string& str)
{
  unsigned char result[MD5_DIGEST_LENGTH];

  MD5((const unsigned char*)str.c_str(), str.size(), result);
  return (base64_encode(result, MD5_DIGEST_LENGTH));
}
