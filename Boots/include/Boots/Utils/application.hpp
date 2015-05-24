#ifndef  BOOTS_APPLICATION_HPP
# define BOOTS_APPLICATION_HPP

# include "singleton.hpp"
# include <list>
# include "../Sync/observatory_network.hpp"
# include "../Sync/observatory_poly.hpp"

class SingleSignalMaster : public Sync::PolymorphicSignalMaster
{
  SINGLETON(SingleSignalMaster)

  SingleSignalMaster() {}
  ~SingleSignalMaster() {}
};

class SignalRemoteSourceManager
{
  SINGLETON(SignalRemoteSourceManager)

  SignalRemoteSourceManager();
  ~SignalRemoteSourceManager();

public:
  void Refresh(void);

private:
  std::list<Sync::SignalRemoteSource*> _remote_sources;
  Sync::ObserverHandler                _obs_handle;
  Sync::Mutex                      _sem;
};

class Application
{
public:
  struct Task
  {
    enum Status
    {
      Continue,
      Stop
    };

    virtual ~Task() {}

    virtual Status Run(void) = 0;
  };

  Application(void);
  ~Application();

  int  Start(int argc, char** argv);
  void Stop(void) { _continue = false; }

  void PushTask(Task* to_add) { _tasks.push_back(to_add); }

  Sync::Signal<void> Finished;

private:
  typedef std::list<Task*> Tasks;

  Tasks _tasks;
  bool  _continue;
};

#endif
