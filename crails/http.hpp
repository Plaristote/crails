#ifndef  HTTP_HPP
# define HTTP_HPP

# include <string>

namespace Http
{
  namespace Url
  {
    std::string Encode(const std::string&);
    std::string Decode(const std::string&);
  }
}

#endif