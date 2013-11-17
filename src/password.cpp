#include "crails/password.hpp"

using namespace std;

string Password::md5(const string& str)
{
  unsigned char result[MD5_DIGEST_LENGTH];

  MD5((const unsigned char*)str.c_str(), str.size(), result);
  return (base64_encode(result, MD5_DIGEST_LENGTH));
}

string Password::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len)
{
  static const string charset =  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
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