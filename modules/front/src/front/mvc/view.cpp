#include <crails/front/mvc/view.hpp>

using namespace std;
using namespace Crails::Front;

View::View()
{}

View::~View()
{
  clear_connected_listeners();
  empty();
}

void View::empty()
{
  if (el->get_parentNode())
    el->get_parentNode()->removeChild(*el);
}

void View::attach(Element& el)
{
  this->el.append_to(*el);
}

void View::add_event_listener(const string& selector, const string& event, DomEventListener::JsCallback callback)
{
  DomEventListener event_listener;

  event_listener.selector = selector;
  event_listener.event    = event;
  event_listener.callback = callback;
  event_listeners.push_back(event_listener);
}

void View::bind_event_listener(Crails::Front::Element& element, const string& event, DomEventListener::JsCallback raw_callback)
{
  client::EventListener* callback = cheerp::Callback(raw_callback);
  pair<string, client::EventListener*> tmp(event, callback);

  element->addEventListener(event.c_str(), callback);
  connected_listeners.push_back(
    ConnectedListeners::value_type(*element, tmp)
  );
}

void View::delegate_events()
{
  clear_connected_listeners();
  for (auto event_listener : event_listeners)
  {
    vector<Element> elements;

    elements = el.find(event_listener.selector);
    for (auto element : elements)
      bind_event_listener(element, event_listener.event, event_listener.callback);
  }
}

void View::clear_connected_listeners()
{
  for (auto connected_listener : connected_listeners)
  {
    auto listener_data = connected_listener.second;
    connected_listener.first->removeEventListener(listener_data.first.c_str(), listener_data.second);
  }
  connected_listeners.clear();
}
