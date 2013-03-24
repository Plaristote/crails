#ifndef  STATE_MACHINE_HPP
# define STATE_MACHINE_HPP

# include "../Sync/observatory.hpp"
# include "exception.hpp"
# include <functional>

template<typename KEY, typename EVENT_TYPE> class StateRunner;

template<typename STATE_TYPE, typename EVENT_TYPE>
class StateMachine
{
  typedef StateRunner<STATE_TYPE, EVENT_TYPE> MyStateRunner;
  typedef std::list<MyStateRunner*>           StateRunners;
public:

  Sync::Signal<void (STATE_TYPE)> StateChanged;

  void SetCurrentState(STATE_TYPE state)
  {
    current_state = state;
    StateChanged.Emit(state);
  }

  STATE_TYPE GetCurrentState(void) const { return (current_state); }

  MyStateRunner* Register(void)
  {
    MyStateRunner* runner = new MyStateRunner;

    runner->SetCurrentState(current_state);
    state_runners.push_back(runner);
    StateChanged.Connect(*runner, &MyStateRunner::SetCurrentState);
    return (runner);
  }

  void FireEvent(EVENT_TYPE event)
  {
    std::for_each(state_runners.begin(), state_runners.end(), [&event](MyStateRunner* state_runner)
    {
      state_runner->Run(event);
    });
  }

private:
  STATE_TYPE   current_state;
  StateRunners state_runners;
};

template<typename KEY, typename EVENT_TYPE>
class StateRunner
{
  friend class StateMachine<KEY, EVENT_TYPE>;

  StateRunner(void) {}
public:
  typedef std::function<void (EVENT_TYPE)> StateCallback;
private:
  struct State
  {
    State(KEY key, StateCallback callback) : key(key), callback(callback) {}

    bool operator==(const KEY& comp) const { return (comp == key); }

    KEY      key;
    StateCallback callback;
  };

  typedef std::list<State> States;

  void SetCurrentState(KEY state)
  {
    current_state = state;
  }

public:
  void PushState(KEY key, StateCallback callback)
  {
    typename States::iterator it = FindState(key);

    if (it == states.end())
      states.push_back(State(key, callback));
    else
      Exception<StateRunner>::Raise(this, "Trying to register the same state twice");
  }

  void RemoveState(KEY key)
  {
    typename States::iterator it = FindState(key);

    if (it != states.end())
      states.erase(it);
  }

  KEY CurrentState(void) const { return (current_state); }

private:
  void Run(EVENT_TYPE event)
  {
    typename States::iterator it = FindState(current_state);

    if (it != states.end())
    {
      State& state = *it;

      state.callback(event);
    }
  }

private:
  inline typename States::iterator FindState(KEY state)
  {
    return (std::find(states.begin(), states.end(), state));
  }

  KEY    current_state;
  States states;
};

#endif
