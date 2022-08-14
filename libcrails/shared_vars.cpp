#include "shared_vars.hpp"
#include "logger.hpp"

using namespace Crails;
using namespace std;

void Crails::output_shared_vars(const SharedVars& vars)
{
  logger << Logger::Debug;
  for (auto item : vars)
  {
    const string& var_name  = item.first;
    const string& type_name = item.second.type().name();

    logger << "shared_vars[\"" << var_name << "\"] -> " << type_name << "\n";
  }
  logger << Logger::endl;
}
