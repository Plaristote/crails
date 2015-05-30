#ifndef  SINGLETON_HPP
# define SINGLETON_HPP

# include "exception.hpp"

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
      Exception<Singleton>::Raise("Was already initialized");
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

# include <thread>
# include "../Sync/semaphore.hpp"
# include <vector>

template<typename TYPE, typename... Args>
class ThreadSingleton
{
  struct Instance
  {
    bool operator==(std::thread::id id) const { return (thread_id == id); }
    
    std::thread::id thread_id;
    TYPE*           ptr;
  };  
public:
  static void Initialize(Args... args)
  {
    Instance instance;
    
    instance.thread_id = std::this_thread::get_id();
    instance.ptr       = new TYPE(args...);
    sem.Wait();
    instances.push_back(instance);
    sem.Post();
  }
  
  static void Finalize(void)
  {
    TYPE*                   ptr = 0;
    {
      Sync::Semaphore::Lock lock(sem);
      auto                  it  = std::find(instances.begin(), instances.end(), std::this_thread::get_id());
      
      if (it != instances.end())
      {
        ptr = it->ptr;
        instances.erase(it);
      }
    }
    delete ptr;
  }
  
  static TYPE* Get(void)
  {
    auto it        = instances.begin();
    auto end       = instances.end();
    auto thread_id = std::this_thread::get_id();
    
    for (; it != end ; ++it)
    {
      if (*it == thread_id)
        return (it->ptr);
    }
    return (0);
  }
  
private:
  static Sync::Semaphore       sem;
  static std::vector<Instance> instances;
};

template<typename TYPE, typename... Args> Sync::Semaphore ThreadSingleton<TYPE, Args...>::sem;
template<typename TYPE, typename... Args> std::vector<typename ThreadSingleton<TYPE, Args...>::Instance> ThreadSingleton<TYPE, Args...>::instances;

#endif
