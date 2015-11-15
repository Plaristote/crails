#ifndef  REGEX_HPP
# define REGEX_HPP

# include <regex.h>
# include <string>

class Regex
{
public:
  struct Exception : public std::exception
  {
    Exception(int retval) : regex_error(retval) {}
    
    int regex_error;
  };
  
  Regex(void) : has_pattern(false) {}
  
  Regex(const std::string& pattern, int flags = 0) : has_pattern(false)
  {
    SetPattern(pattern, flags);
  }
  
  Regex(const Regex& cpy) : has_pattern(false)
  {
    if (cpy.has_pattern)
      SetPattern(cpy.pattern, cpy.flags);
  }
  
  ~Regex(void)
  {
    Free();
  }

  void    SetPattern(const std::string& pattern, int flags = 0);
  int     Match(const std::string& subject, regmatch_t matches[], size_t max_matches, int eflags = 0) const;
  int     Match(const std::string& subject, int eflags = 0) const;

private:
  void    Free(void);
  
  regex_t     regex;
  bool        has_pattern;
  std::string pattern;
  int         flags;
};

#endif