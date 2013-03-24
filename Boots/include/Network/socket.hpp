#ifndef  _NETWORK_SOCKET_HPP_
# define _NETWORK_SOCKET_HPP_

# ifdef _WIN32
#  include <Winsock2.h>
#  include <WS2tcpip.h>
# else
#  include <sys/socket.h>
#  include <netinet/in.h>
# endif

# include <string>
# include <list>
# include "buffer.hpp"
# include "../Sync/observatory_poly.hpp"
# include "../Sync/semaphore.hpp"

namespace Network
{
# ifndef _WIN32
  typedef int    SocketHandle;
# else
  typedef SOCKET SocketHandle;
# endif

  class Socket
  {
    friend class Select;
  public:
    Socket(void) : _socket((SocketHandle)-1), _port(0)
    { }
    Socket(SocketHandle sock, const std::string& ip, unsigned short port) : _socket(sock), _ip(ip), _port(port)
    { }
    Socket(SocketHandle sock) : _socket(sock)
    { }
    ~Socket(void);
 
    bool               Connect(const std::string& hostname, unsigned short port);
    void               Disconnect(void);

    const std::string& Ip(void)          const { return (_ip);     }
    unsigned short     Port(void)        const { return (_port);   }
    bool               IsConnected(void) const { return (_socket != NULL_SOCKET); }
    bool               IsValid(void)     const;

    Buffer&            WriteBuffer()        { return (_buffer_write); }
    Buffer&            ReadBuffer()         { return (_buffer_read);  }
    const Buffer&      WriteBuffer() const  { return (_buffer_write); }
    const Buffer&      ReadBuffer()  const  { return (_buffer_read);  }
   
    Sync::PolymorphicSignal<void (Socket*)> Disconnected;
    Sync::PolymorphicSignal<void (Socket*)> NetworkError;
    Sync::PolymorphicSignal<void (Socket*)> ReceivedData;
    Sync::PolymorphicSignal<void (Socket*)> TransmittedData;
    Sync::PolymorphicSignal<void>           AskingToWrite;

    virtual void       SetSignalsMode(Sync::IPolymorphicSignal::Mode mode);

  protected:
    static void        Close(SocketHandle socket);

    SocketHandle       Handle(void) const   { return (_socket); }
    virtual void       NetworkRead(void);
    virtual void       NetworkWrite(void);    
    
    SocketHandle   _socket;
    std::string    _ip;
    unsigned short _port;
    
    Buffer         _buffer_read, _buffer_write;

    static SocketHandle NULL_SOCKET;

    Sync::Mutex _sem;
  };
}

#endif
