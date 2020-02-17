#ifndef  CRAILS_FRONT_BINDABLE_HPP
# define CRAILS_FRONT_BINDABLE_HPP

# include "signal.hpp"
# include "element.hpp"

namespace Crails
{
  namespace Front
  {
    struct IBindableView : public Crails::Front::Element
    {
      IBindableView() : Crails::Front::Element("div") {}
      IBindableView(const client::String& tagName) : Crails::Front::Element(tagName) {}

      virtual void bind_attributes() = 0;
      virtual void trigger_binding_updates() = 0;
    private:
      IBindableView(const IBindableView&) {}
    };

    struct BindableUpdater
    {
      std::function<void()> func;

      void execute()
      {
        if (func)
          func();
	else
          el.attr(target, getter());
      }

      Crails::Front::Element el;
      const std::string target;
      std::function<std::string (void)> getter;
    };

    class Bindable : public Crails::Listener
    {
    public:
      typedef std::function<std::string (void)> Getter;

      enum BindMode
      {
        StaticBind,
        SignalBind,
        ThrottleBind
      };

    private:
      BindMode               bind_mode = StaticBind;
      Crails::Front::Element element;
      std::string            target;
      Getter                 getter;
      std::function<void()>  updater;

      std::string            signal_name;
      unsigned long          throttle_refresh;
      bool                   enabled = false;
      std::shared_ptr<bool>  enabled_ptr;

    public:
      Bindable() {}
      Bindable(Crails::Front::Element el, const std::string& t, Getter g) : element(el), target(t), getter(g) {}
      Bindable(std::function<void ()> func) : updater(func) {}

      ~Bindable() { disable(); }

      Bindable& bind_to(Crails::Front::Element el) { element = el; return *this; }
      Bindable& on_attribute(const std::string& t, Getter g) { target = t; getter = g; return *this; }
      Bindable& use_mode(BindMode, const std::string& parameter);

      void enable(Crails::Signal<std::string>& signal);
      void disable();
      void update();

    private:
      std::string get_value() const;
      void throttle_schedule();
    };

    class BoundAttributes : public std::vector<Bindable>
    {
    public:
      void update()
      {
        for (Bindable& bindable : *this)
          bindable.update();
      }

      void enable(Crails::Signal<std::string>& signaler)
      {
        disable();
        for (Bindable& bindable : *this)
          bindable.enable(signaler);
      }

      void disable()
      {
        for (Bindable& bindable : *this)
          bindable.disable();
      }
    };
  }
}

#endif
