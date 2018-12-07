#ifndef  CRAILS_FRONT_PROMISE_HPP
# define CRAILS_FRONT_PROMISE_HPP

# include "object.hpp"
# include <vector>

# include "globals.hpp"

namespace Crails
{
  namespace Front
  {
    class Promise : public ObjectImpl<client::Promise<client::Object> >
    {
    public:
      static Promise solved_promise;

      static Promise all(const std::vector<Promise>& promises);

      Promise(std::function<void (std::function<void ()>, std::function<void ()>)> resolver);
      Promise(client::Promise<client::Object>* ptr) : ObjectImpl(ptr) {}
      Promise(client::Object* ptr) : ObjectImpl(ptr) {}

      template<typename T>
      Promise then(T callback)
      {
        Crails::Front::window.set("tintin", *this);
        client::Object* func = cheerp::Callback(callback);
        this->apply("then", func);
        return *this;
      }

      template<typename T>
      Promise _catch(T callback)
      {
        client::Object* func = cheerp::Callback(callback);
        this->apply("catch", func);
        return *this;
      }

    protected:
      void new_promise(client::Object* resolver);

      static void prepare_promise_constructor();
    };

    template<typename RESOLVE_TYPE>
    class PromiseOf : public Promise
    {
    public:
      PromiseOf(std::function<void (std::function<void (RESOLVE_TYPE)>, std::function<void ()>)> resolver) :
        Promise(new client::Promise<client::Object>)
      {
        client::Object* func = cheerp::Callback([this, resolver](client::Object* _resolve, client::Object* _reject)
        {
          this->set("_resolve", _resolve);
          this->set("_reject",  _reject);
          resolver(
            [this](RESOLVE_TYPE value) { this->apply("_resolve", value); },
            [this]() { this->apply("_reject"); }
          );
        });
        new_promise(func);
      }
    };
  }
}

#endif
