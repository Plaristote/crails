#include <Boots/Utils/test.hpp>
#include <Boots/Sync/functorthread.hpp>

using namespace std;
using namespace Sync;

void TestFunctorThread(UnitTest& tester)
{
  tester.AddTest("FunctorThread", "Must properly launch and terminate the std::function object in a thread", []() -> string
  {
    bool                 success        = false;
    FunctorThread<void>* functor_thread = FunctorThread<void>::Create([&success]() { success = true; });

    functor_thread->Launch();
    functor_thread->Join();
    return (success ? "" : "The functor wasn't executed");
  });

  tester.AddTest("FunctorThread", "Must emit a signal sending the proper return value of the functor", []() -> string
  {
    int ret_value = 0;
    FunctorThread<int>* functor_thread = FunctorThread<int>::Create([]() -> int { return (42); });

    functor_thread->Done.Connect([&ret_value](int emitted) { ret_value = emitted; });
    functor_thread->Launch();
    functor_thread->Join();
    return (ret_value == 42 ? "" : "The signal wasn't emitted");
  });
}
