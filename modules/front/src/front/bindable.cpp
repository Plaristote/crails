#include <crails/front/bindable.hpp>
#include <crails/front/globals.hpp>
#include <boost/lexical_cast.hpp>

using namespace Crails::Front;

Bindable& Bindable::use_mode(BindMode mode, const std::string& parameter)
{
  bind_mode = mode;
  switch (mode)
  {
  case ThrottleBind:
    throttle_refresh = boost::lexical_cast<unsigned long>(parameter);
    break ;
  case SignalBind:
    signal_name = parameter;
    break ;
  case StaticBind:
    break ;
  }
  return *this;
}

void Bindable::enable(Crails::Signal<std::string>& signal)
{
  enabled = true;
  switch (bind_mode)
  {
  case StaticBind:
    break ;
  case SignalBind:
    listen_to(signal, [this](const std::string& name) {
      if (name == signal_name)
        update();
    });
    break ;
  case ThrottleBind:
    throttle_schedule();
    break ;
  }
  update();
}

void Bindable::disable()
{
  enabled = false;
  switch (bind_mode)
  {
  case StaticBind:
  case ThrottleBind:
    break ;
  case SignalBind:
    stop_listening();
    break ;
  }
}

void Bindable::update()
{
  if (updater)
    updater();
  else if (!element.is_undefined())
  {
    if (target == "text")
      element.text(get_value());
    else
      element.attr(target, get_value());
  }
}

std::string Bindable::get_value() const
{
  std::string value;

  __asm__("try {");
  value = getter();
  __asm__("} catch (err) { console.warn('failed to update attribute', err); }");
  return value;
}

void Bindable::throttle_schedule()
{
  Crails::Front::window->setTimeout(cheerp::Callback([this]()
  {
    update();
    if (enabled)
      throttle_schedule();
  }), throttle_refresh);
}
