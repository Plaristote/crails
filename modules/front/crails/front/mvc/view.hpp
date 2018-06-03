#ifndef  FUNBONE_VIEW_HPP
# define FUNBONE_VIEW_HPP

# include <cheerp/client.h>
# include <cheerp/clientlib.h>
# include <map>
# include <crails/front/signal.hpp>
# include <crails/front/element.hpp>

namespace Crails
{
  namespace Front
  {
    struct DomEventListener
    {
      typedef std::function<void (client::Event*)> JsCallback;
      std::string selector, event;
      JsCallback  callback;
    };

    class View : public Listener
    {
      typedef std::vector<DomEventListener> EventListeners;
      typedef std::vector<std::pair<client::EventTarget*,std::pair<std::string, client::EventListener*> > > ConnectedListeners;
    public:
      typedef Crails::Front::Element El;

      View();
      virtual ~View();

      virtual void delegate_events();
      virtual void render() { delegate_events(); }

      Element get_element() const { return el; }

      void empty();
      void attach(Element&);

    protected:
      void add_event_listener(const std::string& selector, const std::string& name, DomEventListener::JsCallback callback);
      void bind_event_listener(Element& element, const std::string& event, DomEventListener::JsCallback callback);

      Element el;
    private:
      void clear_connected_listeners();

      EventListeners     event_listeners;
      ConnectedListeners connected_listeners;
    };
  }
}

#endif

