#ifndef  CRAILS_FRONT_SIGNAL_PROMISE_HPP
# define CRAILS_FRONT_SIGNAL_PROMISE_HPP

# include <crails/front/signal.hpp>
# include <crails/front/promise.hpp>

namespace Crails
{
  namespace Front
  {
    class SignalPromise : public Listener, public Promise
    {
      std::function<void (std::function<void()>, std::function<void()>)> make_resolver(Signal<void>* resolve_sig, Signal<void>* reject_sig)
      {
        return [=](std::function<void ()> resolve, std::function<void ()> reject)
        {
          listen_to(*resolve_sig, resolve);
          if (reject_sig != nullptr)
            listen_to(*reject_sig,  reject);
        };
      }

    public:
      SignalPromise(Signal<void>& signal) : Promise(make_resolver(&signal, nullptr))
      {
      }

      SignalPromise(Signal<void>& resolve_sig, Signal<void>& reject_sig) : Promise(make_resolver(&resolve_sig, &reject_sig))
      {
      }
    };
  }
}

#endif
