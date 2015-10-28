#ifndef  COOKIE_DATA_HPP
# define COOKIE_DATA_HPP

# include <string>
# include "datatree.hpp"

namespace Crails
{
  struct CookieData : public DataTree
  {
    void        unserialize(const std::string&);
    std::string serialize(void);

    static const std::string password;
    static const std::string salt;
  };
}

#endif