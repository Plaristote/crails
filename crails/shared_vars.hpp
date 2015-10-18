#ifndef SHARED_VARS_HPP
# define SHARED_VARS_HPP

# include <string>
# include <map>
# include <boost/any.hpp>

namespace Crails
{
  typedef std::map<std::string, boost::any> SharedVars;
}

#endif
