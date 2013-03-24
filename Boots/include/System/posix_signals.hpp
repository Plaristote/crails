#ifndef  SYSTEM_SIGNALS_HPP
# define SYSTEM_SIGNALS_HPP

# include "../Utils/singleton.hpp"
# include "../Sync/observatory_poly.hpp"

namespace Posix
{
  class Signals
  {
    SINGLETON(Signals)

    Signals(void);
    ~Signals(void) {}

    typedef Sync::PolymorphicSignal<void> SignalType;
  public:
    static void Emit(unsigned int pid, int signum);

    SignalType  Hangup;
    SignalType  Interrupt;
    SignalType  Quit;
    SignalType  Trap;
    SignalType  User1;
    SignalType  User2;
    SignalType  BrokenPipe;
    SignalType  Alarm;
    SignalType  ChildStatusChanged;
    SignalType  Continue;
    SignalType  KeyboardStop;
    SignalType  BackgroundTtyRead;
    SignalType  BackgroundTtyWrite;

    Sync::PolymorphicSignal<void (int)> Emitted;
    
  private:
    static void CallbackInitialize(void);
    static void CallbackHandler(int signum);
    void        TriggerSignal(int signum);
  };
}
#endif