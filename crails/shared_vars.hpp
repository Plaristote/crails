#ifndef SHARED_VARS_HPP
# define SHARED_VARS_HPP

# include <string>
# include <map>
# include <boost/any.hpp>
# include "utils/backtrace.hpp" 

namespace Crails
{
  typedef std::map<std::string, boost::any> SharedVars;

  template<typename T>
  T cast(const SharedVars& vars, const std::string& name)
  {
    boost::any var;

    try
    {
      var = vars.at(name);

      return boost::any_cast<T>(var);
    }
    catch (std::out_of_range& e)
    {
      throw boost_ext::out_of_range(e.what());
    }
    catch (boost::bad_any_cast& e)
    {
      throw boost_ext::runtime_error("could not cast `" + name + "` from " + var.type().name() + " to " + typeid(T).name());
    }
  }
}

#endif
