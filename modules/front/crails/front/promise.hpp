#ifndef  CRAILS_FRONT_PROMISE_HPP
# define CRAILS_FRONT_PROMISE_HPP

# include "object.hpp"
# include <vector>

namespace Crails
{
  namespace Front
  {
    class Promise : public ObjectImpl<client::Promise>
    {
    public:
      static Promise solved_promise();
      static Promise all(const std::vector<Promise>& promises);

      Promise(std::function<void (std::function<void ()>, std::function<void ()>)> resolver);
      Promise(client::Promise* ptr) : ObjectImpl(ptr) {}
      Promise(client::Object* ptr) : ObjectImpl(ptr) {}

      template<typename T>
      Promise then(T callback)
      {
        client::EventListener* func = cheerp::Callback(callback);
        (*this)->then(func);
        return *this;
      }

      template<typename T>
      Promise _catch(T callback)
      {
        client::EventListener* func = cheerp::Callback(callback);

        this->apply("catch", Crails::Front::Object(func));
        return *this;
      }

    protected:
      void new_promise(client::Object* resolver);

      static void prepare_promise_constructor();
    };
  }
}

#endif
