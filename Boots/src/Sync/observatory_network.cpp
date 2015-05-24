#include <Boots/Sync/observatory_network.hpp>
#include <iostream>
#include <Boots/Network/socket.hpp>

using namespace Sync;

Signal<void (SignalRemoteSource*)> SignalRemoteSource::NewSignalRemoteSource;
Signal<void (SignalRemoteSource*)> SignalRemoteSource::DestroyedSignalRemoteSource;

void SignalRemoteSource::RetrieveCalls(void)
{
  Network::Buffer& buffer = _socket.ReadBuffer();

  buffer.Lock();
  while (buffer.Length() > sizeof(size_t))
  {
    unsigned short size = *(buffer.Data());

    if (size + sizeof(size) <= buffer.Length())
    {
      Utils::Packet  packet(&(buffer.Data()[sizeof(size)]), size);
      std::string    identifier;
      int            num_calls;
      Sync::ISignal* sig;
      
      packet >> identifier >> num_calls;
      sig = FindSignal(identifier);
      std::cout << "[Sync::SignalRemoteSource][" << this << "] Received " << num_calls << " calls for '" << identifier << '\'' << std::endl;
      while (sig && num_calls > 0)
      {
        sig->PushRecordCall(packet);
        std::cout << sig << ':' << identifier << " now has " << sig->PushedCallCount() << std::endl;
        //sig->ExecuteRecordedCalls();
        num_calls--;
      }
      buffer.Flush(sizeof(size) + (size));
    }
    else
      break ;
  }
  buffer.Unlock();
}

void SignalRemoteSource::RegisterSignal(const std::string& identifier, ISignal& signal)
{
  _signals.push_back(NamedSignal(identifier, signal));
}

void SignalRemoteSource::UnregisterSignal(ISignal& signal)
{
  Signals::iterator it;

  while ((it = std::find(_signals.begin(), _signals.end(), signal)) != _signals.end())
    _signals.erase(it);
}

void SignalRemoteSource::UnregisterSignal(const std::string& identifier)
{
  Signals::iterator it;

  while ((it = std::find(_signals.begin(), _signals.end(), identifier)) != _signals.end())
    _signals.erase(it);
}

ISignal* SignalRemoteSource::FindSignal(const std::string& identifier)
{
  Signals::iterator it  = _signals.begin();
  Signals::iterator end = _signals.end();
  
  for (; it != end ; ++it)
  {
    if (*it == identifier)
      return (&(it->signal));
  }
  std::cerr << "[Sync::SignalRemoteSource][" << this << "] Signal '" << identifier << "' should be registered but isn't" << std::endl;
  return (0);
}

