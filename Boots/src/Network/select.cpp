#include <Boots/Network/network.hpp>
#include <Boots/Utils/exception.hpp>
#include <iostream>
#include <string.h>
#ifndef _WIN32
# include <unistd.h>
# include <sys/select.h>
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

Select::Select(void)
{
  _fd_max      = 0;
  _has_timeout = false;
  FD_ZERO(&_fd_read);
  FD_ZERO(&_fd_write);
  FD_ZERO(&_fd_error);
}

void Select::PushSocket(Socket* sock, unsigned char mode)
{
  Sync::Mutex::Lock lock(_sem);

  StripSocket(sock);
  _clients.push_back(SocketListener(sock, mode));
  if (mode & READ)
  { FD_SET(sock->Handle(), &_fd_read);  }
  if (mode & ERR)
  { FD_SET(sock->Handle(), &_fd_error); }
  if (_fd_max < sock->Handle())
    _fd_max = sock->Handle();
  _observer.Connect(sock->Disconnected, *this, &Select::StripSocket);
}

void Select::StripSocket(Socket* sock)
{
  Sync::Mutex::Lock lock(_sem);
  Sockets::iterator it = std::find(_clients.begin(), _clients.end(), sock);

  if (it != _clients.end())
    _clients.erase(it);
  FD_CLR(sock->Handle(), &_fd_read);
  FD_CLR(sock->Handle(), &_fd_write);
  FD_CLR(sock->Handle(), &_fd_error);
  if (_fd_max == (int)(sock->Handle()))
    RefreshMaxFd();
  _observer.DisconnectAllFrom(sock->Disconnected);
}

void Select::RefreshMaxFd(void)
{
  Sync::Mutex::Lock lock(_sem);
  Sockets::iterator cur = _clients.begin();
  Sockets::iterator end = _clients.end();

  _fd_max = -1;
  for (; cur != end ; ++cur)
  {
    Socket& sock = *(cur->socket);

    if (sock.Handle() > _fd_max)
      _fd_max = (int)(sock.Handle());
  }
}

void Select::Run(void)
{
  _continue = true;
  do
  {
    int            ret;
    struct timeval timeout = _timeout;

    RefreshWriteSet();
    //cout << "Before select" << endl;
    ret = select(_fd_max + 1, &_fd_read, &_fd_write, &_fd_error, (_has_timeout ? &timeout : NULL));
    //cout << "Out of select (" << ret << ")" << endl;

    if (ret == -1)      // Error happened
    {
      // We can't do that for some reason. After a disconnection, the server socket is always invalid.
      // It gets to be valid again though. For some other reason.
      //Network::RaiseLastError(this);
      std::cout << "[Network::Select][" << this << "] " << Network::GetLastErrorMessage() << std::endl;
    }
    if (ret != 0)
      SocketCallbacks();
    SelectLoop.Emit();
  } while (_continue);
}

void Select::SocketCallbacks(void )
{
  Sync::Mutex::Lock lock(_sem);
  Sockets::iterator cur = _clients.begin();
  Sockets::iterator end = _clients.end();
  Sockets::iterator safe;

  while (cur != end)
  {
    Socket& sock   = *(cur->socket);
    int     handle = sock.Handle();

    safe = cur;
    safe++;
    if (FD_ISSET(handle, &_fd_write))
      sock.NetworkWrite();
    if (FD_ISSET(handle, &_fd_error))
      sock.NetworkError.Emit(cur->socket);
    if (FD_ISSET(handle, &_fd_read))
      sock.NetworkRead();
    if (!sock.IsConnected())
      StripSocket(&sock);
    cur = safe;
  }
}

void Select::RefreshWriteSet(void)
{
  Sync::Mutex::Lock lock(_sem);
  Sockets::iterator cur = _clients.begin();
  Sockets::iterator end = _clients.end();

  FD_ZERO(&_fd_write);
  for (; cur != end ; ++cur)
  {
    Socket& sock = *(cur->socket);

    if ((cur->mode & WRITE) && sock.WriteBuffer().Length() > 0)
      FD_SET(sock.Handle(), &_fd_write);
    if (cur->mode & READ)
      FD_SET(sock.Handle(), &_fd_read);
    if (cur->mode & ERR)
      FD_SET(sock.Handle(), &_fd_error);
  }
}

