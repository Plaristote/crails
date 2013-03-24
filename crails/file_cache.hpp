#ifndef  FILE_CACHE_HPP
# define FILE_CACHE_HPP

# include <Boots/Sync/semaphore.hpp>
# include <Boots/Utils/flyweight.hpp>
# include <string>
# include <fstream>

class FileCache : public Flyweight<std::string, std::string>
{
public:
  void         Lock(void)
  {
    semaphore.Wait();
  }

  void         Unlock(void)
  {
    semaphore.Post();
  }

private:
  std::string* CreateInstance(std::string key);

  Sync::Semaphore semaphore;
};

#endif