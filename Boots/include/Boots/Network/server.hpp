#ifndef  _NETWORK_SERVER_HPP_
# define _NETWORK_SERVER_HPP_

# include "socket.hpp"

namespace Network
{
  class Server : public Socket
  {
  public:
    Server(void) {}
    Server(unsigned short port);
    ~Server(void);

    bool           Bind(unsigned short port);
    void           NetworkRead(void);

    Sync::PolymorphicSignal<void (Socket*)> NewConnection;

    virtual void SetSignalsMode(Sync::IPolymorphicSignal::Mode mode)
    {
      NewConnection.SetMode(mode);
      Socket::SetSignalsMode(mode);
    }
  };
}

#endif
