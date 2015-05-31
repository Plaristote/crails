#include <crails/environment.hpp>

using namespace std;

namespace Crails
{
#ifndef SERVER_DEBUG
  string environment = "production";
#else
  string environment = "development";
#endif
}