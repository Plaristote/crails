#include <crails/front/exception.hpp>
#ifdef __CHEERP_CLIENT__
# include <crails/front/globals.hpp>
#endif

namespace Crails
{
  void raise(const std::exception& e)
  {
#ifdef __CHEERP_CLIENT__
    Crails::Front::String what(e.what());

    Crails::Front::window.set("_crails_exception", what);
    __asm__("throw window._crails_exception");
#else
    throw e;
#endif
  }
}
