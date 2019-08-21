#include "crails/tests/background_server.hpp"
#include "crails/server.hpp"
#include "crails/router.hpp"
#include <unistd.h>
#include <signal.h>

using namespace Crails::Tests;
using namespace std;

pid_t Crails::Tests::BackgroundServer::pid = 0;

void BackgroundServer::initialize()
{
  pid = fork();
  if (pid == 0)
    start_server();
}

void BackgroundServer::finalize()
{
  if (pid != 0)
    kill(pid, SIGINT);
}

void BackgroundServer::start_server()
{
  const char* argv[3];

  argv[0] = "";
  argv[1] = "-p";
  argv[2] = "4445";
  Crails::Router::singleton::Finalize(); // will be re-initialized by Launch
  Crails::Server::Launch(3, argv);
  exit(EXIT_SUCCESS);
}
