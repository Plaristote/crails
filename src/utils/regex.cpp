#include <crails/utils/regex.hpp>

using namespace std;

void    Regex::SetPattern(const std::string& pattern, int flags)
{
  int retval;

  Free();
  retval = regcomp(&regex, pattern.c_str(), flags);
  if (retval != 0)
    throw Exception(retval);
  this->pattern = pattern;
  this->flags   = flags;
  has_pattern   = true;
}

int     Regex::Match(const std::string& subject, regmatch_t matches[], size_t max_matches, int eflags) const
{
  return (regexec(&regex, subject.c_str(), max_matches, matches, eflags));
}

int     Regex::Match(const std::string& subject, int eflags) const
{
  return (regexec(&regex, subject.c_str(), 0, 0, eflags));
}

void    Regex::Free(void)
{
  if (has_pattern)
  {
    regfree(&regex);
    has_pattern = false;
  }
}
