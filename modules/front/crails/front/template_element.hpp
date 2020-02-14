#ifndef  CRAILS_FRONT_HTML_TEMPLATE_HPP
# define CRAILS_FRONT_HTML_TEMPLATE_HPP

# include "bindable.hpp"
# include "element.hpp"

namespace Crails
{
  namespace Front
  {
    class BindableEngine
    {
    protected:
      Crails::Front::BoundAttributes bound_attributes;
    public:
      Crails::Signal<std::string> signaler;

      virtual void bind_attributes()
      {
        bound_attributes.enable(signaler);
      }

      virtual void trigger_binding_updates()
      {
        bound_attributes.update();
      }
    };

    class TemplateElement : public Crails::Front::Element, public BindableEngine
    {
    public:
      TemplateElement(const client::String& tagName) : Crails::Front::Element(tagName) {}
      TemplateElement() : Crails::Front::Element("div") {}
    };
  }
}

#endif
