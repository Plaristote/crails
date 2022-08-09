#ifndef  CRAILS_STRING_HPP
# define CRAILS_STRING_HPP

# include <string>
# include <list>

namespace Crails
{
  std::string            generate_random_string(const std::string& charset, unsigned short length);

  std::string            remove_duplicate_characters(const std::string&, char character);
  std::string            underscore(const std::string&);
  std::string            humanize(const std::string&);
  std::string            strip(const std::string&, char character = ' ');
  std::list<std::string> split(const std::string& str, char separator = ' ', bool count_repetitions = false);
  std::string            singularize(const std::string&);
  std::string            pluralize(const std::string&);

  std::string            base64_encode(unsigned char const* str, unsigned int len);
  std::string            base64_encode(const std::string& str_to_encode);
  std::string            base64_decode(const std::string& str_to_decode);
}

#endif
