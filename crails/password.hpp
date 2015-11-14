#ifndef  PASSWORD_HPP
# define PASSWORD_HPP

# include <openssl/md5.h>
# include <string>

namespace Crails
{
  struct Password : public std::string
  {
    Password(void)
    {
    }

    Password(const Password& str) : std::string(str.c_str())
    {
    }
    
    Password(const std::string& str) : std::string(md5(str))
    {
    }
    
    Password(const char* str) : std::string(md5(str))
    {
    }
    
    Password& operator=(const std::string& str)
    {
      std::string::operator=(md5(str));
      return (*this);
    }

    Password& operator=(const Password& str)
    {
      std::string::operator=(str.c_str());
      return (*this);
    }

    bool      operator==(const Password& str) const
    {
      return std::string(str.c_str()) == c_str();
    }

    bool      operator==(const std::string& str) const
    {
      return (md5(str) == c_str());
    }
    
    bool      operator==(const char* str) const
    {
      return (md5(str) == c_str());
    }
    
  private:
    static std::string md5(const std::string& str);
  };
}

#endif
