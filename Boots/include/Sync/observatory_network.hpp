#ifndef  _OBSERVATORY_NETWORK_HPP
# define _OBSERVATORY_NETWORK_HPP

# include "observatory.hpp"
# include "../Network/serializer.hpp"

# define InitNetworkSignalId(sig) \
sig.SetIdentifier(#sig);\
sig.SetMode(Sync::IPolymorphicSignal::NETWORK);

# define InitNetworkSourceSignal(sig, remote_source) \
InitNetworkSignalId(sig);\
sig.AddRemoteSource(*this);

# define InitNetworkListenerSignal(sig, socket) \
InitNetworkSignalId(sig);\
sig.AddRemoteListener(socket);

# define InitNetworkSignal(sig, remote_source, socket) \
InitNetworkSignalId(sig);\
sig.AddRemoteSource(*this);\
sig.AddRemoteListener(socket);

namespace Sync
{
  class SignalRemoteSource
  {
    struct NamedSignal
    {
      NamedSignal(const std::string& id, ISignal& signal) : identifier(id), signal(signal) {}
      
      const std::string identifier;
      ISignal&          signal;
      
      bool operator==(const std::string& comp) const { return (identifier == comp);         }
      bool operator==(const ISignal& signal)   const { return (&signal == &(this->signal)); }
    };
    
    typedef std::list<NamedSignal> Signals;

  public:
    SignalRemoteSource(Network::Socket& socket) : _socket(socket)
    {
      NewSignalRemoteSource.Emit(this);
    }

    ~SignalRemoteSource()
    {
      DestroyedSignalRemoteSource.Emit(this);
    }

    void     RetrieveCalls(void);
    void     RegisterSignal(const std::string& identifier, ISignal& signal);
    void     UnregisterSignal(ISignal& signal);
    void     UnregisterSignal(const std::string& identifier);

    static Signal<void (SignalRemoteSource*)> NewSignalRemoteSource;
    static Signal<void (SignalRemoteSource*)> DestroyedSignalRemoteSource;

  private:
    ISignal* FindSignal(const std::string& identifier);
    
    Network::Socket&     _socket;
    Signals              _signals;
  };
}

#endif
