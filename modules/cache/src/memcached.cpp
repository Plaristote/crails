#include "../crails/memcached.hpp"

using namespace Crails;

Cache::Cache() : memc(0)
{
}

Cache::~Cache()
{
  if (memc)
    memcached_free(memc);
}

void Cache::ensure_initialization()
{
  if (memc == 0)
    memc = memcached(config.c_str(), config.size());
}

void Cache::discard(const std::string& key)
{
  instance.ensure_initialization();
  if (instance.memc)
    memcached_delete(instance.memc, key.c_str(), key.size(), 0);
}

namespace Crails
{
  template<>
  void Cache::serialize<std::string>(std::string source, const char** buffer, size_t& size)
  {
    *buffer = source.c_str();
    size = source.size();
  }

  template<>
  std::string Cache::unserialize<std::string>(const char* buffer, size_t size)
  {
    std::string result;

    return result.assign(buffer, size);
  }
}