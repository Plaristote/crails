#include <Utils/test.hpp>
#include <Utils/state_runner.hpp>

using namespace std;
using namespace Sync;

enum MyStates
{
  STATE_1,
  STATE_2,
  STATE_3
};

enum MyEvents
{
  MY_KEY_PRESSED,
  MY_KEY_UNPRESSED
};

typedef StateMachine<MyStates, MyEvents> MyStateMachine;
typedef StateRunner<MyStates, MyEvents>  MyStateRunner;

void TestStateMachine(UnitTest& tester)
{
  tester.AddTest("StateMachine", "Must send a signal when state is changed", []() -> string
  {
    bool           success = false;
    MyStateMachine my_state_machine;

    my_state_machine.StateChanged.Connect([&success](MyStates) { success = true; });
    my_state_machine.SetCurrentState(STATE_1);
    return (success ? "" : "Signal wasn't triggered");
  });

  tester.AddTest("StateMachine", "StateRunners should inherit the current state of the state machine", []() -> string
  {
    MyStateMachine my_state_machine;
    MyStateRunner* state_runner;

    my_state_machine.SetCurrentState(STATE_1);
    state_runner = my_state_machine.Register();
    if (state_runner->CurrentState() != my_state_machine.GetCurrentState())
      return ("New runners don't inherit the current state of the state machine");
    my_state_machine.SetCurrentState(STATE_2);
    if (state_runner->CurrentState() != STATE_2)
      return ("Runners state isn't updated when StateMachine's state is changed");
    return ("");
  });

  tester.AddTest("StateMachine", "Must fire events to StateRunners that handle the current state", []() -> string
  {
    bool           success = false;
    MyStateMachine my_state_machine;
    MyStateRunner* state_runner = my_state_machine.Register();

    my_state_machine.SetCurrentState(STATE_1);
    state_runner->PushState(STATE_1, [&success](MyEvents event) { success = true; });
    my_state_machine.FireEvent(MY_KEY_PRESSED);
    return (success ? "" : "StateMachine doesn't fire events to runners that implement current state");
  });
}
