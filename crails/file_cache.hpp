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
    void lock(void)
    {
      mutex.lock();
    }

    void unlock(void)
    {
      mutex.unlock();
    }

  private:
    std::shared_ptr<std::string> create_instance(std::string key);

    std::mutex mutex;
  };
}

#endif
