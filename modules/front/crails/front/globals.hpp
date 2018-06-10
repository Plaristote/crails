#ifndef  FRONT_GLOBALS_HPP
# define FRONT_GLOBALS_HPP

# include <crails/front/element.hpp>
# include <crails/front/object.hpp>
# include <crails/front/document.hpp>

namespace Crails
{
  namespace Front
  {
    extern Element                    body;
    extern ObjectImpl<client::Window> window;
    extern Document                   document;
  }
}

#endif
