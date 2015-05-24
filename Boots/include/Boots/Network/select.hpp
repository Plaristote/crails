#ifndef  _NETWORK_SELECT_HPP_
# define _NETWORK_SELECT_HPP_

# include "socket.hpp"

namespace Network
{
  class Select : public Sync::SleepHandle
  {
    struct SocketListener
    {
      SocketListener(Socket* socket, unsigned char mode) : socket(socket), mode(mode) {}
      bool operator==(const Socket* comp) const { return (socket == comp); }

      Socket*       socket;
      unsigned char mode;
    };

    typedef std::list<SocketListener> Sockets;
  public:
    enum Listen
    {
      READ  = 1,
      WRITE = 2,
      ERR   = 4
    };

    Select(void);

    void   Run(void);
    void   Stop(void) { _continue = false; }

    void   PushSocket(Socket* socket, unsigned char mode = (READ | WRITE));
    void   StripSocket(Socket* socket);

    void   SetTimeoutEnabled(bool timeout)    { _has_timeout = timeout; }
    void   SetTimeout(struct timeval timeout) { _timeout     = timeout; }

    Sync::Signal<void> SelectLoop;

  private:
    void   SocketCallbacks(void);
    void   RefreshWriteSet(void);
    void   RefreshMaxFd(void);

    bool                  _continue;
    fd_set                _fd_read, _fd_write, _fd_error;
    int                   _fd_max;
    Sockets               _clients;
    bool                  _has_timeout;
    struct timeval        _timeout;
    Sync::ObserverHandler _observer;
    Sync::Mutex           _sem;
  };
}

#endif
