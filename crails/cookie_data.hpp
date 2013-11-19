#ifndef  COOKIE_DATA_HPP
# define COOKIE_DATA_HPP

# include <Boots/Utils/dynstruct.hpp>
# include <string>

struct CookieData : public DynStruct
{
  void        Unserialize(const std::string&);
  std::string Serialize(void);

  static const std::string password;
  static const std::string salt;
};

#endif