#ifndef  PASSWORD_HPP
# define PASSWORD_HPP

# include <openssl/md5.h>
# include <Boots/Utils/datatree.hpp>
    
struct Password : public std::string
{
  Password(void)
  {
  }
  
  Password(const Data data) : std::string(md5(data.Value()))
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
  static std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
};

#endif