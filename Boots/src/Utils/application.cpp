#include <Boots/Utils/application.hpp>
#include <iostream>

Sync::Signal<void (Sync::IPolymorphicSignal*)> Sync::PolymorphicSignalMaster::NewSignal;
Sync::Signal<void (Sync::IPolymorphicSignal*)> Sync::PolymorphicSignalMaster::DestroyedSignal;

Application::Application() : _continue(true)
{
  SignalRemoteSourceManager::singleton::Initialize();
  SingleSignalMaster::singleton::Initialize();
  {
    SingleSignalMaster* signal_master;

    signal_master = SingleSignalMaster::singleton::Get();
    Sync::PolymorphicSignalMaster::NewSignal.Connect(*signal_master, &Sync::PolymorphicSignalMaster::LinkSignal);
    Sync::PolymorphicSignalMaster::DestroyedSignal.Connect(*signal_master, &Sync::PolymorphicSignalMaster::UnlinkSignal);
  }
}

Application::~Application()
{
  SignalRemoteSourceManager::singleton::Finalize();
  SingleSignalMaster::singleton::Finalize();
}

int Application::Start(int argc, char** argv)
{
  do
  {
    {
      SignalRemoteSourceManager* remote_sources = SignalRemoteSourceManager::singleton::Get();

      if (remote_sources)
        remote_sources->Refresh();
    }

    {
      SingleSignalMaster* signal_master;

      signal_master = SingleSignalMaster::singleton::Get();
      if (signal_master)
        signal_master->Refresh();
    }

    {
      Tasks::iterator it  = _tasks.begin();
      Tasks::iterator end = _tasks.end(); 

      while (it != end)
      {
        if ((*it)->Run() == Task::Continue)
          ++it;
        else
        {
          delete *it;
          it = _tasks.erase(it);
        }
      }
    }
  } while (_continue);
  Finished.Emit();
  return (0);
}

/*
 * SignalRemoteSourceManager
 */
SignalRemoteSourceManager::SignalRemoteSourceManager()
{
  _obs_handle.Connect(Sync::SignalRemoteSource::NewSignalRemoteSource, [this](Sync::SignalRemoteSource* remote_source)
  {
    Sync::Mutex::Lock lock(_sem);

    _remote_sources.push_back(remote_source);
  });
}

SignalRemoteSourceManager::~SignalRemoteSourceManager()
{
}

void SignalRemoteSourceManager::Refresh(void)
{
  Sync::Mutex::Lock lock(_sem);

  for_each(_remote_sources.begin(), _remote_sources.end(), [](Sync::SignalRemoteSource* remote_source)
  {
    remote_source->RetrieveCalls();
  });
}

