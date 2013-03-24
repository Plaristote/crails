#include <System/process.hpp>

using namespace System;
using namespace std;

#ifdef _WIN32
unsigned int System::CurrentPid(void)
{
  return (GetCurrentProcessId());
}
#else
# include <unistd.h>
# include <sys/wait.h>
unsigned int System::CurrentPid(void)
{
  return (getpid());
}

void Process::Stdout(std::ostream& stream)
{
  char reading_buf[256];
  int  nread;
  
  while ((nread = read(pipe_stdout[0], reading_buf, 256)) > 0)
  {
    reading_buf[nread] = 0;
    stream << reading_buf;
  }
}

void Process::Stderr(std::ostream& stream)
{
  char reading_buf[256];
  int  nread;
  
  while ((nread = read(pipe_stderr[0], reading_buf, 256)) > 0)
  {
    reading_buf[nread] = 0;
    stream << reading_buf;
  }
}

Process& Process::operator<<(const string& str)
{
  ::write(pipe_stdin[1], str.c_str(), str.size());
  return (*this);
}

Process& Process::operator<<(char c)
{
  ::write(pipe_stdin[1], &c, 1);
  return (*this);
}

bool Process::Execute(void)
{
  pid_t current_pid;

  pipe(pipe_stdout);
  pipe(pipe_stderr);
  pipe(pipe_stdin);
  current_pid = fork();
  if (current_pid == -1)
    return (false);
  else if (current_pid != 0)
  {
    close(pipe_stdout[1]);
    close(pipe_stderr[1]);
    close(pipe_stdin[0]);
    pid = current_pid;
  }
  else
  {
    // Out
    close(pipe_stdout[0]);
    dup2(pipe_stdout[1], 1);
    
    // Err
    close(pipe_stderr[0]);
    dup2(pipe_stderr[1], 2);
    
    // In
    close(pipe_stdin[1]);
    dup2(pipe_stdin[0], 0);
    
    const char** _argv = new const char*[argv.size() + 2];

    _argv[0] = binpath.c_str();
    for (unsigned int i = 0 ; i < argv.size() ; ++i)
    {
      _argv[i + 1] = argv[i].c_str();
    }
    _argv[argv.size() + 1] = 0;

    execv(binpath.c_str(), (char* const*)_argv);
    exit(-4242);
  }
  return (true);
}

int Process::Join(void)
{
  bool stop = false;

  do
  {
    waitpid(pid, &status, WUNTRACED);
    stop = WIFEXITED(status) || WIFSIGNALED(status) || WCOREDUMP(status);
  } while (!stop);
  close(pipe_stdout[1]);
  return (WEXITSTATUS(status));
}
#endif