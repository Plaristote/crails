#ifndef  CRAILS_TESTS_BACKGROUND_SERVER_HPP
# define CRAILS_TESTS_BACKGROUND_SERVER_HPP

# include <sys/types.h>

namespace Crails
{
  namespace Tests
  {
    class BackgroundServer
    {
      static pid_t pid;
    public:
      static void initialize();
      static void finalize();

    private:
      static void start_server();
    };
  }
}

#endif
