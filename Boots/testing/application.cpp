#include <Utils/test.hpp>
#include <Utils/application.hpp>

using namespace std;

void TestApplication(UnitTest& tester)
{
  tester.AddTest("Application", "Must execute all Polymorphic Signals from the app", []() -> string
  {
    Application                   app;
    Sync::PolymorphicSignal<void> quit_app;
    bool                          success = false;

    quit_app.SetMode(Sync::PolymorphicSignal<void>::QUEUE);
    quit_app.Emit();
    quit_app.Connect([&success]() { success = true; });
    app.Stop();
    app.Start(0, 0);
    return (success ? "" : "Callback wasn't executed");
  });
}

