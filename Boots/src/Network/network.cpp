#include <Network/network.hpp>
#include <Utils/exception.hpp>
#include <iostream>
#include <string.h>
#ifndef _WIN32
# include <unistd.h>
# include <arpa/inet.h>
# include <netdb.h>
#endif

#ifdef _WIN32
# include <io.h>
# define read  _read
# define write _write
#endif

using namespace std;
using namespace Network;

/*
 * Initialization
 */
void Network::Initialize(void)
{
#ifdef _WIN32
    WSADATA wsaData;

    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

void Network::Finalize(void)
{
#ifdef _WIN32
    WSACleanup();
#endif
}

/*
 * Rolling Buffer
 */
void         Buffer::Flush(unsigned int how_much)
{
  Sync::Mutex::Lock lock(_sem);
  copy(&_buffer[how_much], &_buffer[_length], _buffer);
  _length -= how_much;
}

void         Buffer::Push(const char* str, unsigned int length)
{
  Sync::Mutex::Lock lock(_sem);
  unsigned int new_length = _length + length;

  if (new_length > _buffer_length)
    Realloc(new_length);
  copy(str, &str[length], &_buffer[_length]);
  _length = new_length;
}

void         Buffer::Realloc(unsigned int new_length)
{
  Sync::Mutex::Lock lock(_sem);
  char* new_buffer = new char[new_length];

  if (_buffer)
  {
    copy(_buffer, &_buffer[_length], new_buffer);
    delete[] _buffer;
  }
  _buffer        = new_buffer;
  _buffer_length = new_length;
}

/*
 * Server Socket
 */
Server::Server(unsigned short port)
{
  if (!(Bind(port)))
    Exception<Server>::Raise(this, "Couldn't bind server to port");
}

Server::~Server(void)
{
  Sync::Mutex::Lock lock(_sem);

  if (_port != 0)
    Socket::Close(_socket);
}

bool Server::Bind(unsigned short port)
{
  Sync::Mutex::Lock lock(_sem);
  bool                  success = false;

  if (_port != 0)
    cerr << "[Network::Server][" << this << "] is already binded" << endl;
  else
  {
    SocketHandle sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
      cerr << "[Network::Server][" << this << "] was unable to create a socket" << endl;
    else
    {
      struct sockaddr_in serv_addr;
      int                bind_ret;

      memset((char*)&serv_addr, 0, sizeof(serv_addr));
      serv_addr.sin_family      = AF_INET;
      serv_addr.sin_addr.s_addr = INADDR_ANY;
      serv_addr.sin_port        = htons(port);
      bind_ret                  = ::bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
      if (bind_ret != 0)
      {
        Close(sock);
        cerr << "[Network::Server][" << this << "] was unable to bind port " << port << " (" << bind_ret << ')' << endl;
      }
      else
      {
        _socket = sock;
        _port   = port;
        success = true;
        listen(_socket, 10);
      }
    }
  }
  return (success);
}

void Server::NetworkRead(void)
{
  Sync::Mutex::Lock lock(_sem);
  int                   sock;
  struct sockaddr_in    cli_addr;
  socklen_t             cli_len = sizeof(cli_addr);

  sock = accept(_socket, (struct sockaddr*)&cli_addr, &cli_len);
  if (sock < 0)
  {
    cerr << "[Network::Server][" << this << "] accept failed" << endl;
  }
  else
  {
    char    tmp_ip_addr[INET_ADDRSTRLEN];
    Socket* new_socket;

    inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, tmp_ip_addr, INET_ADDRSTRLEN);
    new_socket = new Socket(sock, tmp_ip_addr, cli_addr.sin_port);
    cout << "[Network::Server][" << this << "] New client connected: " << new_socket->Ip() << ':' << new_socket->Port() << endl;

    NewConnection.Emit(new_socket);
  }
}

/*
 * Regular Socket
 */
SocketHandle Socket::NULL_SOCKET = (SocketHandle)(-1);

bool Socket::Connect(const string& hostname, short unsigned int port)
{
  Sync::Mutex::Lock lock(_sem);
  int                   sock = socket(AF_INET, SOCK_STREAM, 0);
  struct hostent*       server;
  struct sockaddr_in    serv_addr;

  if (sock < 0)
    return (false);
  server = gethostbyname(hostname.c_str());
  memset((char*)&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  std::copy((char*)server->h_addr, &((char*)server->h_addr)[server->h_length], (char*)&serv_addr.sin_addr.s_addr);
  serv_addr.sin_port = htons(port);
  if (::connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    return (false);
  _socket = sock;
  return (true);
}

void Socket::SetSignalsMode(Sync::IPolymorphicSignal::Mode mode)
{
  Disconnected.SetMode(mode);
  NetworkError.SetMode(mode);
  ReceivedData.SetMode(mode);
  TransmittedData.SetMode(mode);
  AskingToWrite.SetMode(mode);
}

#ifndef _WIN32
# include <fcntl.h>
# include <errno.h>

bool Socket::IsValid(void) const
{
  return (fcntl(_socket, F_GETFL) != -1 || errno != EBADF);
}
#else
bool Socket::IsValid(void) const
{
  if ((getsockopt(_socket, SOL_SOCKET, SO_ERROR, NULL, 0)) == SOCKET_ERROR)
    return (WSAGetLastError() != WSAENOTSOCK);
  return (true);
}
#endif

void Socket::NetworkRead(void)
{
  Sync::Mutex::Lock lock(_sem);
  int          nread, max_read, total_read;

  total_read                = 0;
  _buffer_read.Lock();
  do
  {
    char*      out          = &(_buffer_read._buffer[_buffer_read._length]);

    nread                   = 0;
    max_read                = _buffer_read._buffer_length - _buffer_read._length;
    if (max_read > 0)
    {
#ifdef _WIN32
      WSABUF buffer;

      buffer.buf = (CHAR*)(out);
      buffer.len = max_read;
      WSARecv(_socket, &buffer, 1, (DWORD*)(&nread), 0, NULL, NULL);
#else
      nread                 = recv(_socket, out, max_read, 0);
#endif
      total_read           += nread;
      _buffer_read._length += nread;
    }
    if (nread == max_read)
      _buffer_read.Realloc(_buffer_read._buffer_length + 32);
  } while (nread == max_read);
  _buffer_read.Unlock();

  if (total_read <= 0)
    Disconnect();
  else
    ReceivedData.Emit(this);
}

void Socket::NetworkWrite(void)
{
  Sync::Mutex::Lock lock(_sem);
  int nwrite;

  _buffer_write.Lock();
#ifdef _WIN32
  WSABUF buffer;

  buffer.buf = (CHAR*)(_buffer_write.Data());
  buffer.len = _buffer_write.Length();

  WSASend(_socket, &buffer, 1, (DWORD*)(&nwrite), 0, NULL, NULL);
#else
  nwrite = send(_socket, _buffer_write.Data(), _buffer_write.Length(), 0);
#endif
  _buffer_write.Flush(nwrite);
  _buffer_write.Unlock();
  if (nwrite > 0)
    TransmittedData.Emit(this);
}

Socket::~Socket()
{
  Disconnect();
}

void Socket::Disconnect(void)
{
  Sync::Mutex::Lock lock(_sem);

  if (_socket != NULL_SOCKET)
  {
    Close(_socket);
    _socket = NULL_SOCKET;
    std::cout << this << " Socket Disconnected" << std::endl;
    Disconnected.Emit(this);
  }
}

#ifdef _WIN32
void Socket::Close(SocketHandle _socket) { closesocket(_socket); }
#else
void Socket::Close(SocketHandle _socket) { close(_socket);       }
#endif
