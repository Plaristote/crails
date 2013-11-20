#include <cstdlib>   // for rand()
#include <cctype>    // for isalnum()   
#include <algorithm> // for back_inserter
#include <string>

static char rand_char()
{
  const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.;:!?%$#()[]{}+-";

  return (charset[std::rand() % charset.size()]);
}

std::string rand_str(std::string::size_type size)
{
  std::string str;
  
  str.reserve(size);
  std::generate_n(std::back_inserter(str), size, rand_char);
  return (str);
}