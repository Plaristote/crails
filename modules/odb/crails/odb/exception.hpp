#ifndef  ODB_EXCEPTION_HPP
# define ODB_EXCEPTION_HPP

# include <crails/utils/backtrace.hpp>
# include <sstream>
# include <iostream> 

namespace ODB
{
  template<typename MODEL>
  void throw_exception(const MODEL& model, const std::string& what)
  {
    std::stringstream stream;

    stream << "object(" << model.get_id() << "): ";
    stream << what;
    throw boost_ext::runtime_error(stream.str().c_str());
  }
}

#endif
