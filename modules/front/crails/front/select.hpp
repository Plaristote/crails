#ifndef  CRAILS_FRONT_SELECT_HPP
# define CRAILS_FRONT_SELECT_HPP

# include <crails/front/element.hpp>
# include <crails/front/events.hpp>
# include <crails/front/signal.hpp>

namespace Crails
{
  namespace Front
  {
    template<typename VALUE_TYPE>
    class Select : public Crails::Front::Element
    {
      JavascriptEvents events;
    public:
      typedef std::map<VALUE_TYPE, std::string> Options;

      Crails::Signal<VALUE_TYPE> changed;

      Select() : Crails::Front::Element("select"), events(static_cast<client::HTMLElement*>(ptr))
      {
        events.on("change", [this]() { changed.trigger(value<VALUE_TYPE>()); });
      }

      void set_options(const Options& options)
      {
        std::vector<Crails::Front::Element> elements;

        elements.reserve(options.size());
        for (const auto& option : options)
          elements.push_back(make_option(option.first, option.second, false));
        html("").inner(elements);
      }

      void set_options(const Options& options, VALUE_TYPE default_value)
      {
        std::vector<Crails::Front::Element> elements;

        elements.reserve(options.size());
        for (const auto& option : options)
          elements.push_back(make_option(option.first, option.second, default_value == option.first));
        html("").inner(elements);
      }

    private:
      Crails::Front::Element make_option(VALUE_TYPE value, const std::string& label, bool selected)
      {
        Crails::Front::Element el("option");

        el.value(value).text(label);
        if (selected)
          el.attr("selected", "selected");
        return el;
      }
    };
  }
}

#endif
