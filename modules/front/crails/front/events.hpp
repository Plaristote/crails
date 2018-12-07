#ifndef  CRAILS_FRONT_EVENTS_HPP
# define CRAILS_FRONT_EVENTS_HPP

# include <cheerp/clientlib.h>
# include <map>
# include <iostream>

namespace Crails
{
  namespace Front
  {
    class JavascriptEvents
    {
      client::EventTarget*                          target;
      std::map<std::string, client::EventListener*> listeners;
    public:
      JavascriptEvents(client::EventTarget* value) : target(value)
      {
      }

      JavascriptEvents(const JavascriptEvents& copy)
      {
        target = copy.target;
      }

      ~JavascriptEvents()
      {
        clear();
      }
      
      JavascriptEvents& operator=(const JavascriptEvents& copy)
      {
        target = copy.target;
        return *this;
      }

      template<typename CALLBACK>
      void on(const std::string& event, CALLBACK callback)
      {
        auto listener = cheerp::Callback(callback);

        if (listeners.count(event)) off(event);
        target->addEventListener(event.c_str(), listener);
        listeners.emplace(event, listener);
      }

      void off(const std::string& event)
      {
        auto it = listeners.find(event);

        if (it != listeners.end())
        {
          target->removeEventListener(it->first.c_str(), it->second);
          listeners.erase(it);
        }
      }

      void clear()
      {
        disconnect();
        listeners.clear();
      }

      void connect()
      {
        disconnect();
        for (auto listener : listeners)
          target->addEventListener(listener.first.c_str(), listener.second);
      }

      void disconnect()
      {
        for (auto listener : listeners)
          target->removeEventListener(listener.first.c_str(), listener.second);
      } 
    };
  }
}

#endif
