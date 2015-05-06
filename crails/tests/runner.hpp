#ifndef  CRAILS_TESTS_RUNNER_HPP
# define CRAILS_TESTS_RUNNER_HPP

# include "helper.hpp"

namespace Crails
{
  namespace Tests
  {
    struct Runner
    {
      void setup();
      void shutdown();
      bool execute();

    private:
      std::list<SP(Helper)> helpers;
    };
  }
}

#endif
