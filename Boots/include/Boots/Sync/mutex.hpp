#ifndef  SYNC_MUTEX_HPP
# define SYNC_MUTEX_HPP

# include <pthread.h>

namespace Sync
{
  class Mutex
  {
    typedef pthread_mutex_t Handle;
  public:
    class Lock
    {
    public:
      Lock(Mutex& mutex) : _mutex(mutex)
      {
        _mutex.Wait();
      }

      ~Lock(void)
      {
        _mutex.Post();
      }

    private:
      Mutex& _mutex;
    };

    Mutex(void);
    ~Mutex(void);

    bool TryWait(void);
    bool Wait(void);
    bool Post(void);

  private:
    Handle handle;
  };
}
#endif
