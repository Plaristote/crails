#ifndef  SEMAPHORE_HPP
# define SEMAPHORE_HPP

# ifdef _WIN32
#  include <Winsock2.h>
#  include <windows.h>
#  include <stdio.h>
# else
#  include <semaphore.h>
#  include <thread>
# endif

namespace Sync
{
  class Semaphore
  {
  # ifdef _WIN32
    typedef HANDLE Handle;
    typedef DWORD  ThreadId; 
  # else
    typedef sem_t           Handle;
    typedef std::thread::id ThreadId;
  # endif
  public:
    class Lock
    {
    public:
      Lock(Semaphore& semaphore) : _semaphore(semaphore)
      {
        if (&_semaphore != 0)
          _semaphore.Wait();
      }

      ~Lock(void)
      {
        if (&_semaphore != 0)
          _semaphore.Post();
      }

    private:
      Semaphore& _semaphore;
    };

    Semaphore(unsigned int initial_count = 1, unsigned int maximum_count = 1);
    ~Semaphore(void);

    bool TryWait(void);
    bool Wait(void);
    bool Post(void);

  private:
    static ThreadId GetCurrentThreadId(void);

    Handle          _handle;
    unsigned int    _max_count;
  };

  class SleepHandle
  {
  public:
    SleepHandle(void) : _semaphore(0, 1)
    {}

    void      Sleep(void)
    {
      _semaphore.Wait();
    }

    void      Wakeup(void)
    {
      _semaphore.Post();
    }

  private:
    Semaphore _semaphore;
  };
}

#endif
