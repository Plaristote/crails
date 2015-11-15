#ifndef  FILE_CACHE_HPP
# define FILE_CACHE_HPP

# include <crails/utils/flyweight.hpp>
# include <string>
# include <fstream>
# include <mutex>

namespace Crails
{
  class FileCache : public Flyweight<std::string, std::string>
  {
  public:
    void         Lock(void)
    {
      mutex.lock();
    }

    void         Unlock(void)
    {
      mutex.unlock();
    }

  private:
    std::string* CreateInstance(std::string key);

    std::mutex mutex;
  };
}

#endif
