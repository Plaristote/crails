#ifndef  CRAILS_ENVIRONMENT_HPP
# define CRAILS_ENVIRONMENT_HPP

# include <string>

namespace Crails
{
  enum Environment
  {
    Production,
    Development,
    Test
  };

  extern Environment environment;
}

#endif
