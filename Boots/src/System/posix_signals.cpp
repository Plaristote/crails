#include <Boots/System/posix_signals.hpp>
#include <unistd.h>

using namespace Posix;
using namespace std;

#ifdef _POSIX_REALTIME_SIGNALS
# include <signal.h>
# include <Boots/System/process.hpp>

Signals::Signals()
{
  static bool callback_initialized = false;
  
  if (!callback_initialized)
  {
    CallbackInitialize();
    callback_initialized = true;
  }

  Hangup.SetMode            (Sync::IPolymorphicSignal::QUEUE);
  Quit.SetMode              (Sync::IPolymorphicSignal::QUEUE);
  Trap.SetMode              (Sync::IPolymorphicSignal::QUEUE);
  User1.SetMode             (Sync::IPolymorphicSignal::QUEUE);
  User2.SetMode             (Sync::IPolymorphicSignal::QUEUE);
  BrokenPipe.SetMode        (Sync::IPolymorphicSignal::QUEUE);
  Alarm.SetMode             (Sync::IPolymorphicSignal::QUEUE);
  ChildStatusChanged.SetMode(Sync::IPolymorphicSignal::QUEUE);
  Continue.SetMode          (Sync::IPolymorphicSignal::QUEUE);
  KeyboardStop.SetMode      (Sync::IPolymorphicSignal::QUEUE);
  BackgroundTtyRead.SetMode (Sync::IPolymorphicSignal::QUEUE);
  BackgroundTtyWrite.SetMode(Sync::IPolymorphicSignal::QUEUE);

  KeyboardStop.Connect([this](void)
  {
    Emit(System::CurrentPid(), SIGSTOP);
  });
}

void Signals::Emit(unsigned int pid, int signum)
{
  kill(pid, signum);
}

void Signals::TriggerSignal(int signum)
{
  switch (signum)
  {
    case SIGHUP:
      Hangup.Emit();
      break ;
    case SIGQUIT:
      Quit.Emit();
      break ;
    case SIGTRAP:
      Trap.Emit();
      break ;
    case SIGUSR1:
      User1.Emit();
      break ;
    case SIGUSR2:
      User2.Emit();
      break ;
    case SIGPIPE:
      BrokenPipe.Emit();
      break ;
    case SIGALRM:
      Alarm.Emit();
      break ;
    case SIGCHLD:
      ChildStatusChanged.Emit();
      break ;
    case SIGCONT:
      Continue.Emit();
      break ;
    case SIGTSTP:
      KeyboardStop.Emit();
      break ;
    case SIGTTIN:
      BackgroundTtyRead.Emit();
      break ;
    case SIGTTOU:
      BackgroundTtyWrite.Emit();
      break ;
  }
  Emitted.Emit(signum);
}

void Signals::CallbackHandler(int signum)
{
  Signals* signals = Signals::singleton::Get();

  if (signals)
    signals->TriggerSignal(signum);
}

void Signals::CallbackInitialize(void)
{
  int signals_handled[] = {
    SIGHUP, SIGQUIT, SIGTRAP, SIGUSR1, SIGUSR2, SIGPIPE, SIGALRM, SIGCHLD, SIGCONT, SIGTSTP, SIGTTIN, SIGTTOU
  };

  for (unsigned short i = 0 ; i < 12 ; ++i)
    signal(signals_handled[i], Signals::CallbackHandler);
}
#else
Signals::Signals() {}
void Signals::Emit(unsigned int, int) {}
void Signals::TriggerSignal(int) {}
void SIgnals::CallbackHandler(int) {}
void Signals::CallbackInitialize(void) {}
#endif
