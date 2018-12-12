#ifndef  CRAILS_FRONT_WINDOW_HPP
# define CRAILS_FRONT_WINDOW_HPP

# include <crails/front/object.hpp>
# include <crails/front/signal.hpp>
# include <crails/front/events.hpp>

namespace Crails
{
  namespace Front
  {
    class Window : public ObjectImpl<client::Window>
    {
    public:
      std::shared_ptr<JavascriptEvents> events;

      Signal<void> resized;

      Window();
    };
  }
}

#endif
