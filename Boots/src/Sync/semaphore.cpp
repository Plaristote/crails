#include <Sync/semaphore.hpp>
#include <iostream>

using namespace Sync;

#ifdef __linux__
Semaphore::Semaphore(unsigned int initial_count, unsigned int max_count)
{
  sem_init(&_handle, 0, initial_count);
  _max_count       = max_count;
}

Semaphore::~Semaphore(void)
{
  sem_destroy(&_handle);
}

Semaphore::ThreadId Semaphore::GetCurrentThreadId(void)
{
  return (std::this_thread::get_id());
}

bool Semaphore::TryWait(void)
{
  return (sem_trywait(&_handle) == 0);
}

bool Semaphore::Wait(void)
{
  return (sem_wait(&_handle) == 0);
}

bool Semaphore::Post(void)
{
  int             value;

  if (!(sem_getvalue(&_handle, &value)))
  {
    if (value < (int)_max_count)
      return (sem_post(&_handle) == 0);
  }
  return (false);
}
#endif

#ifdef _WIN32
Semaphore::Semaphore(unsigned int initial_count, unsigned int maximum_count)
{
  _handle = CreateSemaphore(NULL, initial_count, maximum_count, NULL);
}

Semaphore::~Semaphore(void)
{
  CloseHandle(_handle);
}

Semaphore::ThreadId Semaphore::GetCurrentThreadId(void)
{
  return (::GetCurrentThreadId());
}

bool Semaphore::TryWait(void)
{
  DWORD result;

  if (SelfLock())
    return (true);
  _thread_lock = true;
  _thread_id   = GetCurrentThreadId();
  result       = WaitForSingleObject(_handle, 0);
  return (result != WAIT_FAILED && result != WAIT_TIMEOUT);
}

bool Semaphore::Wait(void)
{
  if (SelfLock())
    return (true);
  _thread_lock = true;
  _thread_id   = GetCurrentThreadId();
  return (WaitForSingleObject(_handle, INFINITE) != WAIT_FAILED);
}

bool Semaphore::Post(void)
{
  _thread_lock = false;
  return (ReleaseSemaphore(_handle, 1, NULL) != 0);
}
#endif

