#include <Utils/test.hpp>
#include <Sync/observatory.hpp>

using namespace std;

void TestsSync(UnitTest& tester)
{
  tester.AddTest("Sync", "Add Callback", []() -> string
  {
    Sync::Signal<void> mysignal;
    bool                      called = false;

    mysignal.Connect([&called]() { called = true; });
    mysignal.Emit();
    if (!called)
      return ("The callback wasn't called");
    return ("");
  });

  tester.AddTest("Sync", "Remove callback", []() -> string
  {
    Sync::Signal<void> mysignal;
    bool                      called = false;
    Sync::ObserverId   id;

    id = mysignal.Connect([&called]() { called = true; });
    mysignal.Disconnect(id);
    mysignal.Emit();
    if (called)
      return ("The callback wasn't removed from observer list");
    return ("");
  });

  tester.AddTest("Sync", "Self-removing callback", []() -> string
  {
    Sync::Signal<void> mysignal;
    Sync::ObserverId   id;
    unsigned char             called = 0;

    id = mysignal.Connect([&id, &mysignal, &called]() { called++; mysignal.Disconnect(id); });
    mysignal.Connect([&called]() { called++; });
    mysignal.Emit();
    if (called != 2)
      return ("Something somewhere went wrong");
    return ("");
  });

  tester.AddTest("Sync", "Observer Handler", []() -> string
  {
    Sync::Signal<void>    mysignal;
    Sync::ObserverHandler handler;
    bool                         called = false;

    handler.Connect(mysignal, [&called]() { called = true; });
    mysignal.Emit();
    if (!called)
      return ("The callback wasn't properly connected");
    handler.DisconnectAll();
    called = false;
    mysignal.Emit();
    if (called)
      return ("The callback wasn't removed properly");
    return ("");
  });

  tester.AddTest("Sync", "Synchronous callback", []() -> string
  {
    Sync::Signal<void> mysignal;
    bool                      called = false;

    mysignal.SetDirect(false);
    mysignal.Connect([&called]() { called = true; });
    mysignal.Emit();
    if (called)
      return ("The callback was directly called instead of being put on queue");
    mysignal.ExecuteRecordedCalls();
    if (!called)
      return ("The queued calls didn't get called by ExecuteRecordedCalls");
    return ("");
  });
}


