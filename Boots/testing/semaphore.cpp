#include <Boots/Utils/test.hpp>
#include <Boots/Sync/semaphore.hpp>
#include <Boots/Sync/functorthread.hpp>

using namespace std;

void TestSemaphore(UnitTest& tester)
{
  tester.AddTest("Semaphore", "Lock object should Wait when created and Post when destroyed", []() -> string
  {
    Sync::Semaphore semaphore;

    {
      Sync::Semaphore::Lock lock(semaphore);

      if (semaphore.TryWait() == true)
        return ("Lock didn't send Wait to the semaphore");
    }
    if (semaphore.TryWait() == false)
      return ("Lock didn't send Post to the semaphore");
    return ("");
  });
}

