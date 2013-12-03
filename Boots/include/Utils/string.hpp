#ifndef  BOOTS_STRING_HPP
# define BOOTS_STRING_HPP

# include <string>
# include <list>

namespace String
{
  std::string            underscore(const std::string&);
  std::string            humanize(const std::string&);
  std::string            strip(const std::string&);  
  std::list<std::string> split(const std::string& str, char separator = ' ');

  std::string            base64_encode(unsigned char const* str, unsigned int len);
  std::string            base64_encode(std::string str_to_encode);
//std::string            base64_decode(std::string str_to_decode); TODO implement decode for base64
}

#endif