#include <Utils/application.hpp>
#include <Utils/cl_options.hpp>
#include <Network/network.hpp>
#include <Sync/functorthread.hpp>

class MyClient : public Sync::SignalRemoteSource
{
public:
  MyClient(Network::Socket* socket) : Sync::SignalRemoteSource(*socket)
  {
    _socket = socket;
    _socket->SetSignalsMode(Sync::IPolymorphicSignal::QUEUE);
    InitNetworkSignal(SetName, *this, socket);
    InitNetworkSignal(SetAge,  *this, socket);
    SetName.Connect(*this, &MyClient::NameSet);
    SetAge.Connect(*this,  &MyClient::AgeSet);
  }

  void NameSet(std::string name)
  {
    std::cout << "NameSet: " << name << std::endl;
  }
  
  void AgeSet(short age)
  {
    std::cout << "AgeSet: " << age << std::endl;
  }

  Sync::PolymorphicSignal<void (std::string)> SetName;
  Sync::PolymorphicSignal<void (short)>       SetAge;
  
private:
  Network::Socket* _socket;
};

#include <sstream>
#include <iostream>
#include "../shared/selectthread.hpp"

using namespace std;
using namespace Sync;

int main(int ac, char** av)
{
  Utils::ClOptions options(ac, av);
  Application      app;
  MyClient*        client;
  unsigned short   port = options.GetValue("-p", "--port", 2048);

  if (ac > 1)
  {
    stringstream stream;
    stream << av[1];
    stream >> port;
  }
  
  FunctorThread<void>::Create([&app, &client, port](void)
  {
    Network::Initialize();
    {
      Network::Socket server;
      SelectThread    read_select, write_select;

      if (server.Connect("localhost", port))
      {
        struct timeval  timeout;
        MyClient        _client(&server);

        server.Disconnected.Connect([&read_select, &write_select, &app](Network::Socket* socket)
        {
          std::cout << "Server Disconnected" << std::endl;
          read_select.StripSocket(socket);
          write_select.StripSocket(socket);
          app.Stop();
        });
        
        client = &_client;
        client->SetName.Emit("Francis");
        app.Finished.Connect(read_select,  &Network::Select::Stop);
        app.Finished.Connect(write_select, &Network::Select::Stop);
        timeout.tv_sec  = 1;
        timeout.tv_usec = 0;
        read_select.SetTimeoutEnabled(true);
        read_select.SetTimeout(timeout);
        read_select.PushSocket(&server,  Network::Select::READ);
        timeout.tv_sec  = 0;
        timeout.tv_usec = 0;
        write_select.SetTimeoutEnabled(true);
        write_select.SetTimeout(timeout);
        write_select.PushSocket(&server, Network::Select::WRITE);

        write_select.SelectLoop.Connect(write_select, &SleepHandle::Sleep);
        server.AskingToWrite.Connect   (write_select, &SleepHandle::Wakeup);
        app.Finished.Connect           (write_select, &SleepHandle::Wakeup);

        write_select.Launch();
        read_select.Launch();
        write_select.Join();
        read_select.Join();
      }
      else
        cout << "Couldn't connect to server" << endl;
      app.Stop();
    }
    Network::Finalize();
  })->Launch();
  return (app.Start(ac, av));
}
