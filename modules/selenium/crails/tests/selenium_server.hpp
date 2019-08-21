#ifndef  CRAILS_TESTS_SELENIUM_SERVER_HPP
# define CRAILS_TESTS_SELENIUM_SERVER_HPP

# include <boost/process.hpp>

namespace Crails
{
  namespace Tests
  {
    class SeleniumServer
    {
      static std::unique_ptr<SeleniumServer> instance;

    public:
      static void initialize()
      {
        instance.reset(new SeleniumServer);
      }

      static void finalize()
      {
        instance.reset(nullptr);
      }

      ~SeleniumServer()
      {
      }

    private:
      SeleniumServer();

      std::unique_ptr<boost::process::child> process;
    };
  }
}

#endif
