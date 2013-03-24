#ifndef  _SYSTEM_PROCESS_HPP
# define _SYSTEM_PROCESS_HPP

# include <vector>
# include <sstream>

namespace System
{
  unsigned int CurrentPid(void);
  
  class Process
  {
  public:
    typedef std::vector<std::string> Args;
    
    Process(const std::string& path, const Args& params) : binpath(path), argv(params)
    {
    }

    bool Execute(void);
    int  Join(void);
    
    void         Stdout(std::ostream& stream);
    void         Stderr(std::ostream& stream);
    
    Process&     operator<<(const std::string& str);
    Process&     operator<<(char c);

  private:
    pid_t        pid;
    std::string  binpath;
    Args         argv;
    int          status;
    
    int          pipe_stdout[2];
    int          pipe_stderr[2];
    int          pipe_stdin[2];
  };
}

#endif