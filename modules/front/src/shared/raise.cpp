#include <crails/raise.hpp>
#ifdef __COMET_CLIENT__
# include <comet/exception.hpp>
#endif

namespace Crails
{
  void raise(const std::exception& e)
  {
#ifdef __CHEERP_CLIENT__
    Comet::raise(e);
#else
    throw e;
#endif
  }
}
