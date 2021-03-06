#include "crails/read_file.hpp"
#include <fstream>

using namespace std;
using namespace Crails;

bool Crails::read_file(const string& filepath, string& out)
{
  ifstream file(filepath.c_str(), std::ios::binary);

  if (file.is_open())
  {
    long           size, begin, end;
    char*          raw;

    begin       = file.tellg();
    file.seekg(0, std::ios::end);
    end         = file.tellg();
    file.seekg(0, std::ios::beg);
    size        = end - begin;
    raw         = new char[size + 1];
    file.read(raw, size);
    file.close();
    out.resize(size);
    std::copy(raw, raw + size, out.begin());
    delete[] raw;
    return true;
  }
  return false;
}
