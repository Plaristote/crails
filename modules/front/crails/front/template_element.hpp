#ifndef  CRAILS_FRONT_HTML_TEMPLATE_HPP
# define CRAILS_FRONT_HTML_TEMPLATE_HPP

# include "bindable.hpp"
# include "element.hpp"

namespace Crails
{
  namespace Front
  {
    class BindableEngineCommon : public IBindableView
    {
    protected:
      Crails::Front::BoundAttributes bound_attributes;
      virtual Crails::Signal<std::string>& get_signaler() = 0;

    public:
      BindableEngineCommon(const client::String& tagName) : IBindableView(tagName) {}
      BindableEngineCommon() {}

      virtual void bind_attributes()
      {
        bound_attributes.enable(get_signaler());
      }

      virtual void trigger_binding_updates()
      {
        bound_attributes.update();
      }
    };

    class BindableEngine : public BindableEngineCommon
    {
    public:
      BindableEngine(const client::String& tagName) : BindableEngineCommon(tagName) {}
      BindableEngine() {}

      Crails::Signal<std::string> signaler;
    protected:
      Crails::Signal<std::string>& get_signaler() { return signaler; }

    };

    class BindableEngineClient : public BindableEngineCommon
    {
    public:
      BindableEngineClient(const client::String& tagName, Crails::Signal<std::string>& s) : BindableEngineCommon(tagName), signaler(s) {}
      BindableEngineClient(Crails::Signal<std::string>& s) : signaler(s) {}

      Crails::Signal<std::string>& signaler;
    protected:
      Crails::Signal<std::string>& get_signaler() { return signaler; }
    };

    class TemplateElement : public BindableEngine
    {
    public:
      TemplateElement(const client::String& tagName) : BindableEngine(tagName) {}
      TemplateElement() {}
    };
  }
}

#endif
