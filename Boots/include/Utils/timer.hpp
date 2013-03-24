#ifndef  TIMER_HPP
# define TIMER_HPP

# ifdef _WIN32
#  include <time.h>
# else
#  include <sys/time.h>
# endif

namespace Utils
{
/*! \class Timer
 * \brief That's a timer. It counts time. */
  class Timer
  {
  public:
    Timer() { Restart(); }

    float  GetElapsedMilliseconds(void) const;
    float  GetElapsedSeconds(void) const      { return (GetElapsedMilliseconds() / 1000); }
    void   Restart(void);
  private:
# ifdef _WIN32
    clock_t        _lastTime;
# else
    struct timeval _lastTime;
# endif
  };
}

#endif
