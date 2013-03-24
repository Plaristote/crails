#ifndef  OBSERVATORY_HPP
# define OBSERVATORY_HPP

# include <list>
# include <queue>
# include <algorithm>
# include <iostream>
# include <functional>

# include "mutex.hpp"
# include "../Utils/serializer.hpp"

# define S1P_TPL                class P1 = void
# define S1P_TFUN               P1 (void)
# define S1P_PARAMS             
# define S1P_VALUES             
# define S1P_RECORD_CON         foe(true)
# define S1P_RECORD_ATTR        bool foe;
# define S1P_RECORD_VAL         
# define S1P_RECORD_SERI        foe
# define S1P_RECORD_UNSERI      foe

# define S2P_TPL                class P1, class P2
# define S2P_TFUN               P1 (P2)
# define S2P_PARAMS             P2 p1
# define S2P_VALUES             p1
# define S2P_RECORD_CON         p1(p1)
# define S2P_RECORD_ATTR        P2 p1;
# define S2P_RECORD_VAL         params.p1
# define S2P_RECORD_SERI        p1
# define S2P_RECORD_UNSERI      packet.operator>> <P2>(p1);

# define S3P_TPL                class P1, class P2, class P3
# define S3P_TFUN               P1 (P2, P3)
# define S3P_PARAMS             P2 p1, P3 p2
# define S3P_VALUES             p1, p2
# define S3P_RECORD_CON         p1(p1), p2(p2)
# define S3P_RECORD_ATTR        P2 p1; P3 p2;
# define S3P_RECORD_VAL         params.p1, params.p2
# define S3P_RECORD_SERI        p1 << p2
# define S3P_RECORD_UNSERI      S2P_RECORD_UNSERI packet.operator>> <P3>(p2);

# define S4P_TPL                class P1, class P2, class P3, class P4
# define S4P_TFUN               P1 (P2, P3, P4)
# define S4P_PARAMS             P2 p1, P3 p2, P4 p3
# define S4P_VALUES             p1, p2, p3
# define S4P_RECORD_CON         p1(p1), p2(p2), p3(p3)
# define S4P_RECORD_ATTR        P2 p1; P3 p2; P4 p3;
# define S4P_RECORD_VAL         params.p1, params.p2, params.p3
# define S4P_RECORD_SERI        p1 << p2 << p3
# define S4P_RECORD_UNSERI      S3P_RECORD_UNSERI packet.operator>> <P4>(p3);

# define S5P_TPL                class P1, class P2, class P3, class P4, class P5
# define S5P_TFUN               P1 (P2, P3, P4, P5)
# define S5P_PARAMS             P2 p1, P3 p2, P4 p3, P5 p4
# define S5P_VALUES             p1, p2, p3, p4
# define S5P_RECORD_CON         p1(p1), p2(p2), p3(p3), p4(p4)
# define S5P_RECORD_ATTR        P2 p1; P3 p2; P4 p3; P5 p4;
# define S5P_RECORD_VAL         params.p1, params.p2, params.p3, params.p4
# define S5P_RECORD_SERI        p1 << p2 << p3 << p4
# define S5P_RECORD_UNSERI      S4P_RECORD_UNSERI packet.operator>> <P5>(p4);

# define S6P_TPL                class P1, class P2, class P3, class P4, class P5, class P6
# define S6P_TFUN               P1 (P2, P3, P4, P5, P6)
# define S6P_PARAMS             P2 p1, P3 p2, P4 p3, P5 p4, P6 p5
# define S6P_VALUES             p1, p2, p3, p4, p5
# define S6P_RECORD_CON         p1(p1), p2(p2), p3(p3), p4(p4), p5(p5)
# define S6P_RECORD_ATTR        P2 p1; P3 p2; P4 p3; P5 p4; P6 p5;
# define S6P_RECORD_VAL         params.p1, params.p2, params.p3, params.p4, params.p5
# define S6P_RECORD_SERI        p1 << p2 << p3 << p4 << p5
# define S6P_RECORD_UNSERI      S5P_RECORD_UNSERI packet.operator>> <P6>(p5);

# define S7P_TPL                class P1, class P2, class P3, class P4, class P5, class P6, class P7
# define S7P_TFUN               P1 (P2, P3, P4, P5, P6, P7)
# define S7P_PARAMS             P2 p1, P3 p2, P4 p3, P5 p4, P6 p5, P7 p6
# define S7P_VALUES             p1, p2, p3, p4, p5, p6
# define S7P_RECORD_CON         p1(p1), p2(p2), p3(p3), p4(p4), p5(p5), p6(p6)
# define S7P_RECORD_ATTR        P2 p1; P3 p2; P4 p3; P5 p4; P6 p5; P7 p6;
# define S7P_RECORD_VAL         params.p1, params.p2, params.p3, params.p4, params.p5, params.p6
# define S7P_RECORD_SERI        p1 << p2 << p3 << p4 << p5 << p6
# define S7P_RECORD_UNSERI      S6P_RECORD_UNSERI packet.operator>> <P7>(p6);

# define DECL_SIGNAL(TPL, TFUN, PARAMS, VALUES, RECORD_CON, RECORD_ATTR, RECORD_VAL, RECORD_SERI, RECORD_UNSERI) \
  template<TPL>                                                         \
  class Signal<TFUN> : virtual public ISignal                           \
  {                                                                     \
    friend class ObserverHandler;                                       \
                                                                        \
    struct InterfaceObserver                                            \
    {                                                                   \
      virtual ~InterfaceObserver() {}                                   \
      virtual void operator()(PARAMS) = 0;                              \
                                                                        \
      ObserverId id;                                                    \
    };                                                                  \
                                                                        \
    template<typename ObserverClass>                                    \
    class Observer : public InterfaceObserver                           \
    {                                                                   \
    public:                                                             \
      typedef void (ObserverClass::*Method)(PARAMS);                    \
                                                                        \
      Observer(ObserverClass& observer, Method method) : _observer(observer), _method(method) {} \
                                                                        \
      void operator()(PARAMS) { (_observer.*_method)(VALUES); }         \
                                                                        \
    private:                                                            \
      ObserverClass& _observer;                                         \
      Method         _method;                                           \
    };                                                                  \
                                                                        \
    class FunctionObserver : public InterfaceObserver                   \
    {                                                                   \
      public:                                                           \
        typedef std::function<void (PARAMS)> Function;                  \
                                                                        \
        FunctionObserver(Function function) : _function(function) {}    \
                                                                        \
        void operator()(PARAMS) { _function(VALUES); }                  \
                                                                        \
      private:                                                          \
        Function _function;                                             \
    };                                                                  \
                                                                        \
    typedef std::list<InterfaceObserver*> Observers;                    \
  public:                                                               \
    struct RecordedCall                                                 \
    {                                                                   \
      RecordedCall(PARAMS) : RECORD_CON {}                              \
      RecordedCall(void) {}                                             \
                                                                        \
      void Serialize(Utils::Packet& packet)                             \
      {                                                                 \
        packet << RECORD_SERI;                                          \
      }                                                                 \
                                                                        \
      void Unserialize(Utils::Packet& packet)                           \
      {                                                                 \
        RECORD_UNSERI;                                        \
      }                                                                 \
                                                                        \
      RECORD_ATTR                                                       \
    };                                                                  \
                                                                        \
    typedef std::queue<RecordedCall> RecordedCalls;                     \
                                                                        \
    Signal(bool direct = true) : _direct(direct)                        \
    { }                             \
                                                                        \
    ~Signal()                                                           \
    {                                                                   \
      typename Observers::iterator it  = _observers.begin();            \
      typename Observers::iterator end = _observers.end();              \
                                                                        \
      for (; it != end ; ++it)                                          \
        delete *it;                                                     \
    }                                                                   \
                                                                        \
    void       SetDirect(bool set)  { _direct = set;    }               \
    bool       IsDirect(void) const { return (_direct); }               \
                                                                        \
    void       Emit(PARAMS)                                             \
    {                                                                   \
      if (_direct)                                                      \
      {                                                                 \
        _iterator = _observers.begin();                                 \
        while (_iterator != _observers.end())                           \
        {                                                               \
          _iterator_updated_during_emit = false;                        \
          (**_iterator)(VALUES);                                        \
          if (_iterator_updated_during_emit == false)                   \
            _iterator++;                                                \
        }                                                               \
      }                                                                 \
      else                                                              \
      {                                                                 \
        Mutex::Lock lock(_semaphore);                               \
                                                                        \
        _recordedCalls.push(RecordedCall(VALUES));                      \
      }                                                                 \
    }                                                                   \
                                                                        \
    template<typename ObserverClass>                                    \
    ObserverId Connect(ObserverClass& observerInstance, typename Observer<ObserverClass>::Method method) \
    {                                                                   \
      InterfaceObserver* observer = new Observer<ObserverClass>(observerInstance, method); \
                                                                        \
      return (AddObserver(observer));                                   \
    }                                                                   \
                                                                        \
    ObserverId Connect(typename FunctionObserver::Function function)    \
    {                                                                   \
      InterfaceObserver* observer = new FunctionObserver(function);     \
                                                                        \
      return (AddObserver(observer));                                   \
    }                                                                   \
                                                                        \
    void       Disconnect(ObserverId id)                                \
    {                                                                   \
      typename Observers::iterator toDel = _observers.begin();          \
      typename Observers::iterator end   = _observers.end();            \
                                                                        \
      for (; toDel != end ; ++toDel)                                    \
      {                                                                 \
        if (*toDel == id)                                               \
        {                                                               \
          InterfaceObserver* observer = *toDel;                         \
                                                                        \
          if (toDel == _iterator)                                       \
          {                                                             \
            _iterator_updated_during_emit = true;                       \
            _iterator = _observers.erase(toDel);                        \
          }                                                             \
          else                                                          \
            _observers.erase(toDel);                                    \
          delete observer;                                              \
          break ;                                                       \
        }                                                               \
      }                                                                 \
    }                                                                   \
                                                                        \
    void       DisconnectAll(void)                                      \
    {                                                                   \
      typename Observers::iterator toDel = _observers.begin();          \
      typename Observers::iterator end   = _observers.end();            \
                                                                        \
      while (_observers.begin() != end)                                 \
      {                                                                 \
        delete *(_observers.begin());                                   \
        _observers.erase(_observers.begin());                           \
      }                                                                 \
      _iterator = _observers.end();                                     \
    }                                                                   \
                                                                        \
    inline int ObserverCount(void) const                                \
    {                                                                   \
      return (_observers.size());                                       \
    }                                                                   \
                                                                        \
    void       PushRecordCall(Utils::Packet& packet)                    \
    {                                                                   \
      Mutex::Lock lock(_semaphore);                                 \
      RecordedCall params;                                              \
                                                                        \
      params.Unserialize(packet);                                       \
      _pushedCalls.push(params);                                        \
    }                                                                   \
                                                                        \
    bool       FetchRecordCall(Utils::Packet& packet)                   \
    {                                                                   \
      Mutex::Lock lock(_semaphore);                                 \
      RecordedCall& params = _recordedCalls.front();                    \
                                                                        \
      params.Serialize(packet);                                         \
      _recordedCalls.pop();                                             \
      return (_recordedCalls.size() > 0);                               \
    }                                                                   \
                                                                        \
    int        RecordCallCount(void)                                    \
    {                                                                   \
      Mutex::Lock lock(_semaphore);                                 \
      return (_recordedCalls.size());                                   \
    }                                                                   \
									\
    int        PushedCallCount(void)                                    \
    {                                                                   \
      Mutex::Lock lock(_semaphore);                                 \
      return (_pushedCalls.size());                                     \
    }                                                                   \
                                                                        \
    void       ExecuteRecordedCalls(void)                               \
    {                                                                   \
      Mutex::Lock lock(_semaphore);                                 \
                                                                        \
      FuncExecuteRecordedCalls(_recordedCalls);                         \
      FuncExecuteRecordedCalls(_pushedCalls);                           \
    }                                                                   \
                                                                        \
    void       FuncExecuteRecordedCalls(RecordedCalls& calls)           \
    {                                                                   \
      while (calls.size())                                              \
      {                                                                 \
        RecordedCall& params = calls.front();                           \
                                                                        \
        _iterator = _observers.begin();                                 \
        while (_iterator != _observers.end())                           \
        {                                                               \
          _iterator_updated_during_emit = false;                        \
          (**_iterator)(RECORD_VAL);                                    \
          if (_iterator_updated_during_emit == false)                   \
            _iterator++;                                                \
        }                                                               \
        calls.pop();                                                    \
      }                                                                 \
    }                                                                   \
                                                                        \
    void       BackupRecordedCalls(bool on_off)                         \
    {                                                                   \
      if (on_off)                                                       \
        _backedupCalls = _recordedCalls;                                \
      else                                                              \
        _recordedCalls = _backedupCalls;                                \
    }                                                                   \
                                                                        \
  private:                                                              \
    ObserverId AddObserver(InterfaceObserver* observer)                 \
    {                                                                   \
      ObserverId id = observer;                                         \
                                                                        \
      _observers.push_back(observer);                                   \
      return (id);                                                      \
    }                                                                   \
                                                                        \
    typename Observers::iterator _iterator;                             \
    bool                         _iterator_updated_during_emit;         \
    Observers                    _observers;                            \
    RecordedCalls                _recordedCalls;                        \
    RecordedCalls                _backedupCalls;                        \
    RecordedCalls                _pushedCalls;                          \
    bool                         _direct;                               \
    Mutex                    _semaphore;                            \
  };

namespace Sync
{
  typedef void* ObserverId;
  
  class ObserverHandler;
  
  class ISignal
  {
  public:
    virtual void ExecuteRecordedCalls(void)       = 0;
    virtual void PushRecordCall(Utils::Packet&)   = 0;
    virtual bool FetchRecordCall(Utils::Packet&)  = 0;
    virtual int  RecordCallCount(void)            = 0;
    virtual int  PushedCallCount(void)            = 0;
    virtual void BackupRecordedCalls(bool on_off) = 0;
    virtual void SetDirect(bool)                  = 0;
    virtual int  ObserverCount(void) const        = 0;
  };

  template<class p1 = void>
  class Signal : virtual public ISignal
  {
    friend class ObserverHandler;

    struct InterfaceObserver
    {
      virtual ~InterfaceObserver() {}
      virtual void operator()() = 0;

      ObserverId id;
    };

    class FunctionObserver : public InterfaceObserver
    {
      public:
        typedef std::function<void (void)> Function;

        FunctionObserver(Function function) : _function(function) {}

        void operator()()
        {
          _function();
        }

      private:
        Function _function;
    };

    template<class ObserverClass>
    class Observer : public InterfaceObserver
    {
    public:
      typedef void (ObserverClass::*Method)();

      Observer(ObserverClass& observer, Method method) : _observer(observer), _method(method) {}

      void operator()() { (_observer.*_method)(); }

    private:
      ObserverClass& _observer;
      Method         _method;
    };

    typedef std::list<InterfaceObserver*> Observers;
  public:
    struct RecordedCall
    {
      void Serialize(Utils::Packet& packet)   {}
      void Unserialize(Utils::Packet& packet) {}
      bool byte;
    };
    typedef std::queue<RecordedCall> RecordedCalls;

    Signal(bool direct = true) : _direct(direct)
    { }

    ~Signal()
    {
      typename Observers::iterator it  = _observers.begin();
      typename Observers::iterator end = _observers.end();

      for (; it != end ; ++it)
        delete *it;
    }
    
    void       SetDirect(bool set)  { _direct = set;    }
    bool       IsDirect(void) const { return (_direct); }

    void       Emit()
    {
      if (_direct)
      {
        _iterator = _observers.begin();
        while (_iterator != _observers.end())
        {
          _iterator_updated_during_emit = false;
          (**_iterator)();
          if (_iterator_updated_during_emit == false)
            _iterator++;
        }
      }
      else
        _recordedCalls.push(RecordedCall());
    }

    template<class ObserverClass>
    ObserverId Connect(ObserverClass& observerInstance, typename Observer<ObserverClass>::Method method)
    {
      InterfaceObserver* observer = new Observer<ObserverClass>(observerInstance, method);

      return (AddObserver(observer));
    }

    ObserverId Connect(typename FunctionObserver::Function function)
    {
      InterfaceObserver* observer = new FunctionObserver(function);

      return (AddObserver(observer));
    }

    void       Disconnect(ObserverId id)
    {
      typename Observers::iterator toDel = _observers.begin();
      typename Observers::iterator end   = _observers.end();

      for (; toDel != end ; ++toDel)
      {
        if (*toDel == id)
        {
          InterfaceObserver* observer = *toDel;

          if (toDel == _iterator)
          {
            _iterator_updated_during_emit = true;
            _iterator = _observers.erase(toDel);
          }
          else
            _observers.erase(toDel);
          delete observer;
          break ;
        }
      }
    }

    void       DisconnectAll(void)
    {
      typename Observers::iterator toDel = _observers.begin();
      typename Observers::iterator end   = _observers.end();

      while (_observers.begin() != end)
      {
        delete *(_observers.begin());
        _observers.erase(_observers.begin());
      }
      _iterator = _observers.end();
    }

    inline int ObserverCount(void) const
    {
      return (_observers.size());
    }
    
    void       ExecuteRecordedCalls(void)
    {
      Sync::Mutex::Lock lock(_semaphore);

      FuncExecuteRecordedCalls(_recordedCalls);
      FuncExecuteRecordedCalls(_pushedCalls);
    }

    void       FuncExecuteRecordedCalls(RecordedCalls& calls)
    {
      while (calls.size())
      {
        _iterator = _observers.begin();
        while (_iterator != _observers.end())
        {
          _iterator_updated_during_emit = false;
          (**_iterator)();
          if (_iterator_updated_during_emit == false)
            _iterator++;
        }
        calls.pop();
      }
    }

    void       PushRecordCall(Utils::Packet& packet)
    {
      Mutex::Lock lock(_semaphore);
      RecordedCall    params;

      params.Unserialize(packet);
      _pushedCalls.push(params);
    }
    
    bool       FetchRecordCall(Utils::Packet& packet)
    {
      Mutex::Lock lock(_semaphore);
      RecordedCall&   params = _recordedCalls.front();

      params.Serialize(packet);
      _recordedCalls.pop();
      return (_recordedCalls.size() > 0);
    }
    
    int        RecordCallCount(void)
    {
      Mutex::Lock lock(_semaphore);
      return (_recordedCalls.size());
    }

    int        PushedCallCount(void)
    {
      Mutex::Lock lock(_semaphore);
      return (_pushedCalls.size());
    }

    void       BackupRecordedCalls(bool on_off)
    {
      if (on_off)
        _backedupCalls = _recordedCalls;
      else
        _recordedCalls = _backedupCalls;
    }

  private:
    ObserverId AddObserver(InterfaceObserver* observer)
    {
      ObserverId id = observer;

      _observers.push_back(observer);
      return (id);
    }

    typename Observers::iterator _iterator;
    bool                         _iterator_updated_during_emit;
    Observers                    _observers;
    RecordedCalls                _recordedCalls;
    RecordedCalls                _backedupCalls;
    RecordedCalls                _pushedCalls;
    bool                         _direct;
    Mutex                        _semaphore;
  };
 
  //DECL_SIGNAL(S1P_TPL, S1P_TFUN, S1P_PARAMS, S1P_VALUES, S1P_RECORD_CON, S1P_RECORD_ATTR, S1P_RECORD_VAL, S1P_RECORD_SERI, S1P_RECORD_UNSERI) 
  DECL_SIGNAL(S2P_TPL, S2P_TFUN, S2P_PARAMS, S2P_VALUES, S2P_RECORD_CON, S2P_RECORD_ATTR, S2P_RECORD_VAL, S2P_RECORD_SERI, S2P_RECORD_UNSERI)
  DECL_SIGNAL(S3P_TPL, S3P_TFUN, S3P_PARAMS, S3P_VALUES, S3P_RECORD_CON, S3P_RECORD_ATTR, S3P_RECORD_VAL, S3P_RECORD_SERI, S3P_RECORD_UNSERI)
  DECL_SIGNAL(S4P_TPL, S4P_TFUN, S4P_PARAMS, S4P_VALUES, S4P_RECORD_CON, S4P_RECORD_ATTR, S4P_RECORD_VAL, S4P_RECORD_SERI, S4P_RECORD_UNSERI)
  DECL_SIGNAL(S5P_TPL, S5P_TFUN, S5P_PARAMS, S5P_VALUES, S5P_RECORD_CON, S5P_RECORD_ATTR, S5P_RECORD_VAL, S5P_RECORD_SERI, S5P_RECORD_UNSERI)
  DECL_SIGNAL(S6P_TPL, S6P_TFUN, S6P_PARAMS, S6P_VALUES, S6P_RECORD_CON, S6P_RECORD_ATTR, S6P_RECORD_VAL, S6P_RECORD_SERI, S6P_RECORD_UNSERI)
  DECL_SIGNAL(S7P_TPL, S7P_TFUN, S7P_PARAMS, S7P_VALUES, S7P_RECORD_CON, S7P_RECORD_ATTR, S7P_RECORD_VAL, S7P_RECORD_SERI, S7P_RECORD_UNSERI)

  class ObserverHandler
  {
    struct IObserverPair
    {
      virtual ~IObserverPair() {};
      virtual void Disconnect(void) = 0;
      virtual bool operator==(const ISignal& comp) const = 0;
    };

    template<class C>
    struct ObserverPair : public IObserverPair
    {
      ObserverPair(C& signal, ObserverId id) : signal(signal), id(id) {}
      void Disconnect(void)                      { signal.Disconnect(id);     }
      bool operator==(const ISignal& comp) const { return (&signal == &comp); }
      C&         signal;
      ObserverId id;
    };
    
    typedef std::list<IObserverPair*> Observers;
  public:
    ~ObserverHandler()
    {
      DisconnectAll();
    }
    
    void       DisconnectAll(void)
    {
      std::for_each(_observers.begin(), _observers.end(), [](IObserverPair* observer)
      {
        observer->Disconnect();
        delete observer;
      });
      _observers.clear();
    }

    void       DisconnectAllFrom(const ISignal& signal)
    {
      Observers::iterator it  = _observers.begin();
      Observers::iterator end = _observers.end();

      while (it != end)
      {
        IObserverPair* observer = *it;

        if ((*observer) == signal)
        {
          observer->Disconnect();
          delete observer;
          it = _observers.erase(it);
        }
        else
          ++it;
      }
    }
    
    template<class C>
    inline void Add(C& signal, ObserverId id)
    {
      _observers.push_back(new ObserverPair<C>(signal, id));
    }
    
    template<class C, typename ObserverClass>
    inline void Connect(C& signal, ObserverClass& observerInstance, typename C::template Observer<ObserverClass>::Method method)
    {
      ObserverId id = signal.Connect(observerInstance, method);
      
      Add(signal, id);
    }

    template<class C>
    inline void Connect(C& signal, typename C::FunctionObserver::Function function)
    {
      ObserverId id = signal.Connect(function);

      Add(signal, id);
    }

  private:
    Observers _observers;
  };
}

#endif
