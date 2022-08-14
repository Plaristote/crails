#include "getenv.hpp"
#include <cstdlib>

using namespace std;

namespace Crails
{
  string getenv(const string& varname, const string& def)
  {
    const char* value = std::getenv(varname.c_str());

    if (value)
      return value;
    return def;
  }
}
