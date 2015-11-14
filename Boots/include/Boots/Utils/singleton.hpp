#ifndef  SINGLETON_HPP
# define SINGLETON_HPP

# include "backtrace.hpp"

# define SINGLETON(type) \
public:\
  typedef Singleton<type> singleton; \
private:\
  friend class Singleton<type>;

template<typename TYPE, typename... Args>
class Singleton
{
public:
  static void Initialize(Args... args)
  {
    if (!(ptr))
      ptr = new TYPE(args...);
    else
      throw boost_ext::runtime_error("Was already initialized");
  }

  static void Finalize(void)
  {
    if (ptr)
    {
      delete ptr;
      ptr = 0;
    }
  }

  static TYPE*  Get(void) { return (ptr); }

private:
  static TYPE* ptr;
  static int   val;
};

template<typename TYPE, typename... Args> TYPE* Singleton<TYPE, Args...>::ptr = 0;

#endif
