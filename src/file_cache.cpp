#include "crails/file_cache.hpp"

using namespace std;

std::string* FileCache::CreateInstance(std::string key)
{
  ifstream file(key.c_str(), std::ios::binary);
  
  if (file.is_open())
  {
    long           size, begin, end;
    char*          raw;
    std::string*   instance;

    begin       = file.tellg();
    file.seekg(0, std::ios::end);
    end         = file.tellg();
    file.seekg(0, std::ios::beg);
    size        = end - begin;
    raw        = new char[size + 1];
    file.read(raw, size);
    file.close();
    instance = new std::string;
    instance->resize(size);
    std::copy(raw, raw + size, instance->begin());
    return (instance);
  }
  return (0);
}
