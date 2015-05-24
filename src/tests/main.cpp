#include "crails/tests/runner.hpp"
#include "crails/router.hpp"
#include "crails/databases.hpp"

using namespace std;
using namespace Crails;
using namespace Crails::Tests;

int main(int, char**)
{
  Runner runner;
  int    exit_status = 1;

  Databases::singleton::Initialize();
  Router::singleton::Initialize();
  Router::singleton::Get()->initialize();
  runner.setup();
  if (runner.execute())
    exit_status = 0;
  runner.shutdown();
  Router::singleton::Finalize();
  Databases::singleton::Finalize();
  return exit_status;
}
