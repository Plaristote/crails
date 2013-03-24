#ifndef  _SYNC_LIST_HPP_
# define _SYNC_LIST_HPP_

# include <list>
# include <algorithm>
# include "mutex.hpp"

namespace Sync
{
  template<typename T>
  class List : private std::list<T>
  {
  public:
    void PushBack(T& var)
    {
      Mutex::Lock lock(semaphore);

      push_back(var);
    }

    void RemoveOne(const T& var)
    {
      Remove(var, false);
    }

    void RemoveAll(const T& var)
    {
      Remove(var, true);
    }

    void ForEach(std::function<void (T&)> functor)
    {
      Mutex::Lock lock(semaphore);

      std::for_each(std::list<T>::begin(), std::list<T>::end(), functor);
    }

    void ForEach(std::function<void (const T&)> functor) const
    {
      Mutex::Lock lock(semaphore);

      std::for_each(std::list<T>::begin(), std::list<T>::end(), functor);
    }

    void Clear(void)
    {
      Mutex::Lock lock(semaphore);

      std::list<T>::clear();
    }

  private:
    void Remove(const T& var, bool all)
    {
      Mutex::Lock                 lock(semaphore);
      typename std::list<T>::iterator it   = std::list<T>::begin();
      typename std::list<T>::iterator last = std::list<T>::end();

      while (it != last)
      {
        if (*it == var)
        {
          it = erase(it);
          if (!all)
            break ;
        }
        else
          ++it;
      }
    }

    Mutex semaphore;
  };
}

#endif
