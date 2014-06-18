#include <Sync/thread.hpp>

using namespace Sync;

#ifdef _WIN32
void Thread::Launch(void)
{
  _handle = CreateThread(NULL, 0, &Thread::CallbackLaunch, this, 0, NULL);
  if (_handle == NULL)
    LaunchFailed();
}

void Thread::Join(void)
{
  WaitForSingleObject(_handle, INFINITE);
}
#else
void Thread::Launch(void)
{
  if ((pthread_create(&_handle, NULL, &Thread::CallbackLaunch, this)) < 0)
    LaunchFailed();
}

void Thread::Join(void)
{
  pthread_join(_handle, 0);
}
#endif

void Thread::LaunchFailed(void)
{
}
