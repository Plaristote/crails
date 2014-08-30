#ifndef  CRAILS_MONGODB_EXCEPTION_HPP
# define CRAILS_MONGODB_EXCEPTION_HPP

# include <crails/backtrace.hpp>
# include <string>

namespace MongoDB
{
  class Exception : public boost_ext::exception
  {
  public:
    Exception(const std::string& message) : message(message)
    {
    }
    
    const char* what(void) const throw()
    {
      return (message.c_str());
    }
    
  private:
    std::string message;
  };
}

#endif