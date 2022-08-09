#include "crails/tests/runner.hpp"
#include "crails/router.hpp"
#include "crails/databases.hpp"
#include "crails/server.hpp"
#include <crails/logger.hpp>

using namespace std;
using namespace Crails;
using namespace Crails::Tests;

Server* test_server = nullptr;

int main(int, char**)
{
  Server server;
  Runner runner;
  int    exit_status = 1;

  test_server = &server;
  Crails::environment = "test";
  Router::singleton::Initialize();
  Router::singleton::Get()->initialize();
  runner.setup();
  if (runner.execute())
    exit_status = 0;
  runner.shutdown();
  Router::singleton::Finalize();
  return exit_status;
}
