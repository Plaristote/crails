#ifndef  _OBSERVATORY_POLY_HPP_
# define _OBSERVATORY_POLY_HPP_

# include "observatory.hpp"
# include "observatory_network.hpp"

# define TPL_POL_SIGNAL(PARAMS_DECL, PARAMS) \
template<PARAMS_DECL>\
  class PolymorphicSignal<PARAMS> : public IPolymorphicSignal, public Signal<PARAMS>\
  {\
    typedef std::list<Network::Socket*> RemoteListeners;\
  public:\
    PolymorphicSignal(void)\
    {\
      SetMode(DIRECT);\
      PolymorphicSignalMaster::NewSignal.Emit(this);\
    }\
\
    PolymorphicSignal(const std::string id, Mode mode = DIRECT)\
    {\
      SetIdentifier(id);\
      SetMode(mode);\
      PolymorphicSignalMaster::NewSignal.Emit(this);\
    }\
\
    ~PolymorphicSignal(void)\
    {\
      PolymorphicSignalMaster::DestroyedSignal.Emit(this);\
    }\
\
    void SetIdentifier(const std::string& id)\
    {\
      _identifier = id;\
    }\
\
    void SetMode(Mode mode)\
    {\
      _mode = mode;\
      Signal<PARAMS>::SetDirect(mode == DIRECT ? true : false);\
    }\
\
    void Refresh(void)\
    {\
      switch (_mode)\
      {\
      case NETWORK:\
        NetworkAllRemoteCalls();\
      case QUEUE:\
        Signal<PARAMS>::ExecuteRecordedCalls();\
      default:\
        break ;\
      }\
    }\
\
    void AddRemoteListener(Network::Socket* socket) { _remotes.push_back(socket); }\
    void RemoveRemoteListener(Network::Socket* socket) { _remotes.remove(socket); }\
\
    void AddRemoteSource(SignalRemoteSource& source)\
    {\
      source.RegisterSignal(_identifier, *this);\
    }\
\
    void RemoveRemoteSource(SignalRemoteSource& source)\
    {\
      source.UnregisterSignal(*this);\
    }\
\
    Mode GetMode(void)       const { return (_mode);       }\
    const std::string& GetIdentifier(void) const { return (_identifier); }\
\
  private:\
    void     NetworkAllRemoteCalls(void)\
    {\
      RemoteListeners::iterator it  = _remotes.begin();\
      RemoteListeners::iterator end = _remotes.end();\
\
      for (; it != end ; ++it)\
        NetworkPushCalls(*it);\
    }\
\
    void     NetworkPushCalls(Network::Socket* socket)\
    {\
      if (Signal<PARAMS>::RecordCallCount() > 0)\
      {\
        Utils::Packet  packet;\
\
        packet << _identifier << Signal<PARAMS>::RecordCallCount();\
        Signal<PARAMS>::BackupRecordedCalls(true);\
        while (Signal<PARAMS>::RecordCallCount())\
          Signal<PARAMS>::FetchRecordCall(packet);\
        Network::WritePacket(packet, socket);\
        Signal<PARAMS>::BackupRecordedCalls(false);\
      }\
    }\
\
    std::string     _identifier; \
    Mode            _mode; \
    RemoteListeners _remotes; \
  };

namespace Sync
{
  class PolymorphicSignalMaster;

  class IPolymorphicSignal : virtual public ISignal
  {
    friend class PolymorphicSignalMaster;
  protected:
    IPolymorphicSignal() : _master(0) { }

  public:
    virtual ~IPolymorphicSignal();

    enum Mode
    {
      DIRECT,
      QUEUE,
      NETWORK
    };

    virtual void               Refresh(void) = 0;
    virtual void               SetIdentifier(const std::string&) = 0;
    virtual const std::string& GetIdentifier(void) const = 0;
    virtual void               SetMode(Mode) = 0;
    virtual Mode               GetMode(void) const = 0;
    virtual void               AddRemoteSource(SignalRemoteSource&) = 0;
    virtual void               RemoveRemoteSource(SignalRemoteSource&) = 0;
    virtual void               AddRemoteListener(Network::Socket*) = 0;
    virtual void               RemoveRemoteListener(Network::Socket*) = 0;

  private:
    PolymorphicSignalMaster*   _master;
  };

  class PolymorphicSignalMaster
  {
    typedef std::list<IPolymorphicSignal*> Signals;
  public:
    PolymorphicSignalMaster()
    {
    }

    ~PolymorphicSignalMaster();

    PolymorphicSignalMaster& operator<<(IPolymorphicSignal* signal);
    void                     LinkSignal(IPolymorphicSignal* signal) { *this << signal; }
    void                     UnlinkSignal(IPolymorphicSignal* signal);
    void                     Refresh(void);

    static Signal<void (IPolymorphicSignal*)> NewSignal;
    static Signal<void (IPolymorphicSignal*)> DestroyedSignal;

  private:
    Signals   _signals;
    Mutex _semaphore;
  };

  template<class p1 = void>
  class PolymorphicSignal : public IPolymorphicSignal, public Signal<void>
  {
    typedef std::list<Network::Socket*> RemoteListeners;
  public:
    PolymorphicSignal(void)
    {
      SetMode(DIRECT);
      PolymorphicSignalMaster::NewSignal.Emit(this);
    }

    PolymorphicSignal(const std::string id, Mode mode = DIRECT)
    {
      SetIdentifier(id);
      SetMode(mode);
      PolymorphicSignalMaster::NewSignal.Emit(this);
    }

    ~PolymorphicSignal()
    {
      PolymorphicSignalMaster::DestroyedSignal.Emit(this);
    }

    void SetIdentifier(const std::string& id)
    {
      _identifier = id;
    }

    void SetMode(Mode mode)
    {
      _mode = mode;
      Signal<void>::SetDirect(mode == DIRECT ? true : false);
    }

    void Refresh(void)
    {
      switch (_mode)
      {
      case NETWORK:
        NetworkAllRemoteCalls();
      case QUEUE:
        Signal<void>::ExecuteRecordedCalls();
      default:
        break ;
      }
    }

    void AddRemoteListener(Network::Socket* socket) { _remotes.push_back(socket); }
    void RemoveRemoteListener(Network::Socket* socket) { _remotes.remove(socket); }

    void AddRemoteSource(SignalRemoteSource& source)
    {
      source.RegisterSignal(_identifier, *this);
    }

    void RemoveRemoteSource(SignalRemoteSource& source)
    {
      source.UnregisterSignal(*this);
    }

    Mode GetMode(void)       const { return (_mode);       }
    const std::string& GetIdentifier(void) const { return (_identifier); }

  private:
    void     NetworkAllRemoteCalls(void)
    {
      RemoteListeners::iterator it  = _remotes.begin();
      RemoteListeners::iterator end = _remotes.end();

      for (; it != end ; ++it)
        NetworkPushCalls(*it);
    }

    void     NetworkPushCalls(Network::Socket* socket)
    {
      if (Signal<void>::RecordCallCount() > 0)
      {
        Utils::Packet  packet;

        packet << _identifier << Signal<void>::RecordCallCount();
        Signal<void>::BackupRecordedCalls(true);
        while (Signal<void>::RecordCallCount())
          Signal<void>::FetchRecordCall(packet);
        Network::WritePacket(packet, socket);
        Signal<void>::BackupRecordedCalls(false);
      }
    }

    std::string     _identifier;
    Mode            _mode;
    RemoteListeners _remotes;
  };
  
  //TPL_POL_SIGNAL(S1P_TPL, S1P_TFUN)
  TPL_POL_SIGNAL(S2P_TPL, S2P_TFUN)
  TPL_POL_SIGNAL(S3P_TPL, S3P_TFUN)
  TPL_POL_SIGNAL(S4P_TPL, S4P_TFUN)
  TPL_POL_SIGNAL(S5P_TPL, S5P_TFUN)
  TPL_POL_SIGNAL(S6P_TPL, S6P_TFUN)
  TPL_POL_SIGNAL(S7P_TPL, S7P_TFUN)
}


#endif
