#include "any_cast.hpp"
#include <iostream>

namespace Crails
{
  std::string any_cast(const boost::any& val)
  {
    try {
      return std::string(boost::any_cast<const char*>(val));
    } catch (...) {}
    return boost::any_cast<std::string>(val);
  }
}
