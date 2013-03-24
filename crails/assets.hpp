#ifndef  ASSETS_HPP 
# define ASSETS_HPP

# include <Boots/Utils/directory.hpp>
# include <Boots/Utils/regex.hpp>

# define ASSETS_PATH          std::string("../app/assets/")
# define COMPILED_ASSETS_PATH std::string("../public/assets/")

# include <string>
# include <sstream>

class Assets
{
public:
  struct Exception : public std::exception
  {
    Exception(const std::string& filename, const std::stringstream& errstream)
    {
      message   = "Couldn't compile '" + filename + '\'';
      backtrace = errstream.str();
    }
    
    Exception(const std::string& message) : message(message)
    {
    }
    
    const char* what() const throw()
    {
      return (message.c_str());
    }
    
    std::string message;
    std::string backtrace;
  };
  
  static void Precompile(void);
private:
  static void Cleanup(void);
  
  static void Coffeescript(void);
  static void UglifyJS(void);
};

#endif