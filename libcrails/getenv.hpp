#ifndef  CRAILS_GETENV_HPP
# define CRAILS_GETENV_HPP

# include <string>
# include <boost/lexical_cast.hpp>

namespace Crails
{
  std::string getenv(const std::string& varname, const std::string& def = "");

  template<typename T>
  T getenv_as(const std::string& varname, const T value)
  {
    std::string str_val = getenv(varname);

    if (str_val == "")
      return value;
    return boost::lexical_cast<T>(str_val);
  }
}

#endif
