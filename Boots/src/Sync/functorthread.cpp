#include <Sync/functorthread.hpp>

namespace Sync
{
  template<>
  void FunctorThread<void>::RunFunctor(void)
  {
    functor();
    Done.Emit();
  }
}
