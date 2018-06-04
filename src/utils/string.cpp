#include <crails/utils/string.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <iostream>

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
