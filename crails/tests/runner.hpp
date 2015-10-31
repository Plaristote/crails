#ifndef  CRAILS_TESTS_RUNNER_HPP
# define CRAILS_TESTS_RUNNER_HPP

# include "helper.hpp"
# include <memory>

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
      std::list<std::shared_ptr<Helper> > helpers;
    };
  }
}

#endif
