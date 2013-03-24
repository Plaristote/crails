#ifndef  _SYNC_THREAD_HPP_
# define _SYNC_THREAD_HPP_

# ifdef _WIN32
#  include <Winsock2.h>
#  include <windows.h>
# else
#  include <pthread.h>
# endif

namespace Sync
{
  class Thread
  {
  public:
    virtual ~Thread(void) {}
    void          Launch(void);
    void          Join(void);

  protected:
    virtual void  Run(void) = 0;

  private:
    virtual void  LaunchFailed(void);

# ifdef _WIN32
    static DWORD WINAPI CallbackLaunch(LPVOID lpParam)
    {
      reinterpret_cast<Thread*>(lpParam)->Run();
      return (0);
    }

    HANDLE    _handle;  
# else
    static void* CallbackLaunch(void* arg)
    {
      reinterpret_cast<Thread*>(arg)->Run();
      return (0);
    }

    pthread_t _handle;
# endif
  };
}

#endif
