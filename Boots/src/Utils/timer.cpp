#include <Utils/timer.hpp>

using namespace Utils;

#ifdef _WIN32

float Timer::GetElapsedMilliseconds(void) const
{
  clock_t currentTime;
  clock_t elapsedTime;

  currentTime = clock();
  elapsedTime = currentTime - _lastTime;
  return ((elapsedTime * CLOCKS_PER_SEC) / 1000.f);
}


void Timer::Restart()
{
  _lastTime = clock();
}

#else

float Timer::GetElapsedMilliseconds(void) const
{
  struct timeval tv;

  gettimeofday(&tv, 0);
  tv.tv_sec  -= _lastTime.tv_sec;
  tv.tv_usec -= _lastTime.tv_usec;
  return (tv.tv_sec * 1000.f + tv.tv_usec / 1000.f);
}


void Timer::Restart()
{
  gettimeofday(&_lastTime, 0);
}

#endif
