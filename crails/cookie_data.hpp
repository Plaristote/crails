#ifndef  COOKIE_DATA_HPP
# define COOKIE_DATA_HPP

# include <Boots/Utils/dynstruct.hpp>
# include <string>

namespace Crails
{
  struct CookieData : public DynStruct
  {
    void        unserialize(const std::string&);
    std::string serialize(void);

    static const std::string password;
    static const std::string salt;
  };
}

#endif