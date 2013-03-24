#include <Utils/application.hpp>
#include <Utils/cl_options.hpp>
#include <Network/network.hpp>
#include <Sync/functorthread.hpp>
#include <iostream>

#include "../shared/selectthread.hpp"

class MyClient : public Sync::SignalRemoteSource
{
public:
  MyClient(Network::Socket* socket) : Sync::SignalRemoteSource(*socket)
  {
    std::cout << "A client connected" << std::endl;
    _socket = socket;
    _socket->SetSignalsMode(Sync::IPolymorphicSignal::QUEUE);
    InitNetworkSignal(SetName, *this, socket);
    InitNetworkSignal(SetAge,  *this, socket);

    SetName.Connect([this](std::string new_name)
    {
      SetAge.Emit(12);
      if (new_name == "Francis")
        SetName.Emit("Francis#2");
      std::cout << "Received name -> " << new_name << std::endl;
    });
  }

  Sync::PolymorphicSignal<void (std::string)> SetName;
  Sync::PolymorphicSignal<void (short)>       SetAge;

private:
  Network::Socket* _socket;
};

class MyApplication : public Application::Task
{
  typedef std::list<MyClient*> Clients;
public:
  MyApplication(void)
  {
    NewConnection.SetMode(Sync::PolymorphicSignal<void (Network::Socket*)>::QUEUE);
    NewConnection.Connect([this](Network::Socket* socket) { _clients.push_back(new MyClient(socket)); });
  }

  Application::Task::Status Run(void)
  {
    return (Application::Task::Continue);
  }

  Sync::PolymorphicSignal<void (Network::Socket*)> NewConnection;

private:
  Clients _clients;
};

#include <sstream>
#include <System/posix_signals.hpp>
using namespace std;
using namespace Sync;

int main(int ac, char** av)
{
  Utils::ClOptions options(ac, av);
  Application      app;
  MyApplication    my_app;
  int              port = options.GetValue("-p", "--port", 2048);

  { // Posix Signal Handle
    Posix::Signals::singleton::Initialize();
    Posix::Signals* posix_sigs = Posix::Signals::singleton::Get();

    posix_sigs->Interrupt.Connect([&app](void) { app.Stop(); });
    app.Finished.Connect([](void) { Posix::Signals::singleton::Finalize(); });
  } // End Posix Signal Handle

  FunctorThread<void>::Create([&app, &my_app, port](void)
  {
    Network::Initialize();
    {
      //Network::Select selector;
      SelectThread read_select, write_select;
      Network::Server server;
      struct timeval  timeout;

      if (!(server.Bind(port)))
      {
        cerr << "Impossible to bind port " << port << endl;
        app.Stop();
      }
      server.SetSignalsMode(Sync::IPolymorphicSignal::QUEUE);
      server.NewConnection.Connect([&read_select, &write_select](Network::Socket* socket)
      {
        socket->Disconnected.Connect([&read_select, &write_select](Network::Socket* socket)
        {
          std::cout << "Client disconnected" << std::endl;
          read_select.StripSocket(socket);
          write_select.StripSocket(socket);
        });
        socket->AskingToWrite.Connect(write_select, &SleepHandle::Wakeup);
      });
      server.NewConnection.Connect(my_app.NewConnection, &PolymorphicSignal<void (Network::Socket*)>::Emit);
      server.NewConnection.Connect([&read_select] (Network::Socket* socket) { read_select.PushSocket(socket);  });
      server.NewConnection.Connect([&write_select](Network::Socket* socket) { write_select.PushSocket(socket); });
      app.Finished.Connect(read_select, &Network::Select::Stop);
      app.Finished.Connect(write_select, &Network::Select::Stop);

      write_select.SelectLoop.Connect(write_select, &SleepHandle::Sleep);
      server.AskingToWrite.Connect   (write_select, &SleepHandle::Wakeup);
      app.Finished.Connect           (write_select, &SleepHandle::Wakeup);
      
      timeout.tv_sec  = 1;
      timeout.tv_usec = 0;
      read_select.SetTimeoutEnabled(true);
      read_select.SetTimeout(timeout);
      read_select.PushSocket(&server);
      timeout.tv_sec  = 0;
      write_select.SetTimeoutEnabled(true);
      write_select.SetTimeout(timeout);

      read_select.Launch();
      write_select.Launch();
      read_select.Join();
      write_select.Join();
    }
    Network::Finalize();
  })->Launch();

  return (app.Start(ac, av));
}
