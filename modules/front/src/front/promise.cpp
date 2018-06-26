#include <crails/front/promise.hpp>
#include <crails/front/globals.hpp>

using namespace std;
using namespace Crails::Front;

Promise::Promise(std::function<void (std::function<void ()>, std::function<void ()>)> resolver)
{
  client::Object* func = cheerp::Callback([this, resolver](client::Object* _resolve, client::Object* _reject)
  {
    this->set("_resolve", _resolve);
    this->set("_reject",  _reject);
    resolver(
      [this]() { this->apply("_resolve"); },
      [this]() { this->apply("_reject"); }
    );
  });
  new_promise(func);
}

void Promise::new_promise(client::Object* resolver)
{
  prepare_promise_constructor();
  ptr = *(Crails::Front::window.apply("_crails_new_promise", resolver));
}

void Promise::prepare_promise_constructor()
{
  static bool is_prepared = false;

  if (!is_prepared)
  {
    client::eval("window._crails_new_promise = function(r) { return new Promise(r); }");
    is_prepared = true;
  }
}

Promise Promise::all(const std::vector<Promise>& promises)
{
  Crails::Front::ObjectImpl<client::Array> array;

  for (auto promise : promises)
    array->push(*promise);
  return *(Crails::Front::window["Promise"].apply("all", array));
}
