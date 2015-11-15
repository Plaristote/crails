#ifndef  CRAILS_STRING_HPP
# define CRAILS_STRING_HPP

# include <string>
# include <list>

namespace Crails
{
  std::string            generate_random_string(const std::string& charset, unsigned short length);

  std::string            underscore(const std::string&);
  std::string            humanize(const std::string&);
  std::string            strip(const std::string&);  
  std::list<std::string> split(const std::string& str, char separator = ' ');

  std::string            base64_encode(unsigned char const* str, unsigned int len);
  std::string            base64_encode(std::string str_to_encode);
//std::string            base64_decode(std::string str_to_decode); TODO implement decode for base64
}

#endif
