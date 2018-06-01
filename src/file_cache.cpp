#include "crails/file_cache.hpp"
#include "crails/read_file.hpp"

using namespace std;
using namespace Crails;

std::string* FileCache::CreateInstance(std::string key)
{
  string* instance = new string;

  if (read_file(key, *instance))
    return instance;
  delete instance;
  return 0;
}
