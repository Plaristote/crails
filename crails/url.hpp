#ifndef  CRAILS_URL_HPP
# define CRAILS_URL_HPP

# include <string>

namespace Crails
{
  struct Url
  {
    static std::string encode(const std::string&);
    static std::string decode(const std::string&);
    static Url         from_string(const std::string&);
    std::string        to_string() const;

    bool           ssl = false;
    std::string    host;
    unsigned short port = 80;
    std::string    target;
  };
}

#endif
