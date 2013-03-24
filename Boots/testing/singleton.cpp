#include <Utils/test.hpp>
#include <Utils/singleton.hpp>

using namespace std;

void TestSingleton(UnitTest& tester)
{
  tester.AddTest("Singleton", "Must throw exception if Initialized several time", []() -> string
  {
    try
    {
      Singleton<std::string>::Initialize();
      Singleton<std::string>::Initialize();
    }
    catch (Exception<Singleton<std::string> > e)
    {
      return ("");
    }
    return ("Didn't throw the exception");
  });

  tester.AddTest("Singleton", "Must reinstantiate if finalized and re-initialized", []() -> string
  {
    try
    {
      Singleton<int>::Initialize();
      Singleton<int>::Finalize();
      if (Singleton<int>::Get() != 0)
        return ("Finalize didn't remove the instance");
      Singleton<int>::Initialize();
      if (Singleton<int>::Get() == 0)
        return ("Initialize didn't create the instance");
    }
    catch (Exception<Singleton<std::string> > e)
    {
      return ("An exception was thrown");
    }
    return ("");
  });

}
