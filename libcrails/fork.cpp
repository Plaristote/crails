#include "server.hpp"
#include "logger.hpp"

using namespace Crails;

static char** duplicate_arguments(int argc, const char** argv)
{
  char** arguments = new char*[argc + 1];

  for (unsigned short i = 0 ; i < argc ; ++i)
  {
    size_t length = strlen(argv[i]);

    arguments[i] = new char[length];
    strncpy(arguments[i], argv[i], length);
    arguments[i][length] = 0;
  }
  arguments[argc] = 0;
  return arguments;
}

void Server::fork(int argc, const char** original_argv)
{
  pid_t  pid  = ::fork();
  char** argv = duplicate_arguments(argc, original_argv);

  switch (pid)
  {
    default:
      execve(argv[0], argv, nullptr);
    case -1:
      logger << Logger::Error << "!! Failed to restart server" << Logger::endl;
    case 0:
      break ;
  }
}
