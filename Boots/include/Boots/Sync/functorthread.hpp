#ifndef  _SYNC_FUNCTOR_THREAD_HPP_
# define _SYNC_FUNCTOR_THREAD_HPP_

# include "thread.hpp"
# include "list.hpp"
# include <functional>
# include "observatory.hpp"

namespace Sync
{
  template<typename RETURN> struct FunctorThreadFoo
  {
    typedef void (callback_fptr)(RETURN);
  };

  template<> struct FunctorThreadFoo<void>
  {
    typedef void (callback_fptr)();
  };

  template<typename RETURN>
  class FunctorThread : public Thread
  {
  public:
    void         Set(std::function<RETURN (void)> functor)
    {
      this->functor = functor;
    }

  protected:
    virtual void Run(void)
    {
      RunFunctor();
      delete this;
    }

  public:
    Sync::Signal<typename FunctorThreadFoo<RETURN>::callback_fptr> Done;

    static FunctorThread* Create(std::function<RETURN (void)> functor)
    {
      FunctorThread* ptr = new FunctorThread;

      ptr->Set(functor);
      return (ptr);
    }

  private:
    FunctorThread(void)
    {
    }

    virtual ~FunctorThread(void)
    {
    }

    std::function<RETURN (void)> functor;

    void RunFunctor(void)
    {
      Done.Emit(functor());
    }
  };

  template<>
  void FunctorThread<void>::RunFunctor(void);
}

#endif
