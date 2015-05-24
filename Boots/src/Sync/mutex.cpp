#include <Boots/Sync/mutex.hpp>

using namespace Sync;

Mutex::Mutex(void)
{
  pthread_mutexattr_t mta;

  handle = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutexattr_init(&mta);
  pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&handle, &mta);
}

Mutex::~Mutex()
{
  pthread_mutex_destroy(&handle);
}

bool Mutex::TryWait(void)
{
  return (pthread_mutex_trylock(&handle) == 0);
}

bool Mutex::Wait(void)
{
  return (pthread_mutex_lock(&handle) == 0);
}

bool Mutex::Post(void)
{
  return (pthread_mutex_unlock(&handle) == 0);
}
