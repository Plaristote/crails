#ifndef  CRAILS_ANY_CAST_HPP
# define CRAILS_ANY_CAST_HPP

# include <boost/any.hpp>
# include <string>

namespace Crails
{
  std::string any_cast(const boost::any& val);
}

#endif
