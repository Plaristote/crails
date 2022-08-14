#include "file_cache.hpp"
#include "read_file.hpp"

using namespace std;
using namespace Crails;

shared_ptr<string> FileCache::create_instance(std::string key)
{
  shared_ptr<string> instance = make_shared<string>();

  if (read_file(key, *instance))
    return instance;
  return nullptr;
}
