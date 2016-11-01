#include <crails/front/globals.hpp>

namespace Crails
{
  namespace Front
  {
    Crails::Front::Element          body(client::document.get_body());
    ObjectImpl<client::EventTarget> window((client::EventTarget*)&client::window);
  }
}
