#ifndef  FUNBONE_SIGNAL_HPP
# define FUNBONE_SIGNAL_HPP

# include <map>
# include <vector>

namespace Crails
{
  class ISignal
  {
  public:
    virtual void disconnect(unsigned long) = 0;
  private:
  };

  class Listener
  {
  public:
    virtual ~Listener()
    {
      stop_listening();
    }

    template<typename SIGNAL>
    void listen_to(SIGNAL& signal, typename SIGNAL::Callback callback)
    {
      unsigned long id = signal.connect(callback);

      connections.push_back(std::pair<ISignal*,unsigned long>(&signal, id));
    }

    void stop_listening(ISignal* ptr = 0)
    {
      for (auto it = connections.begin() ; it != connections.end() ;)
      {
        if (ptr == 0 || it->first == ptr)
        {
          it->first->disconnect(it->second);
          it = connections.erase(it);
        }
        else
          it++;
      }
    }

  private:
    std::vector<std::pair<ISignal*,unsigned long> > connections;
  };

  template<typename ...Args>
  class Signal : public ISignal
  {
    friend class Listener;
    typedef std::function<void (Args...)> Callback;
  public:
    Signal() : listener_i(0)
    {
    }

    Signal(const Signal& cpy) : listener_i(0)
    {
    }

    Signal& operator=(const Signal& cpy)
    {
      return *this;
    }

    void trigger(Args... args)
    {
      for (auto slot : callbacks)
        slot.second(args...);
    }

    unsigned long connect(Callback callback)
    {
      callbacks.insert(std::pair<unsigned long, Callback>(listener_i, callback));
      listener_i++;
      return listener_i - 1;
    }

    void disconnect(unsigned long i)
    {
      for (auto it = callbacks.begin() ; it != callbacks.end() ; ++it)
      {
        if (it->first == i)
        {
          callbacks.erase(it);
          break ;
        }
      }
    }

  private:
    int listener_i;
    std::map<unsigned long, Callback> callbacks;
  };

  template<>
  class Signal<void> : public ISignal
  {
    friend class Listener;
    typedef std::function<void (void)> Callback;
  public:
    Signal() : listener_i(0)
    {
    }

    Signal(const Signal& cpy) : listener_i(0)
    {
    }

    Signal& operator=(const Signal& cpy)
    {
      return *this;
    }

    void trigger()
    {
      for (auto slot : callbacks)
        slot.second();
    }

    unsigned long connect(Callback callback)
    {
      callbacks.insert(std::pair<unsigned long, Callback>(listener_i, callback));
      listener_i++;
      return listener_i - 1;
    }

    void disconnect(unsigned long i)
    {
      for (auto it = callbacks.begin() ; it != callbacks.end() ; ++it)
      {
        if (it->first == i)
        {
          callbacks.erase(it);
          break ;
        }
      }
    }

  private:
    int listener_i;
    std::map<unsigned long, Callback> callbacks;
  };

}

#endif
