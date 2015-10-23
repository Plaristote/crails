#include <crails/logger.hpp>

using namespace Crails;

int main(int argc, char** argv)
{
  logger << Logger::Info << "Task example" << Logger::endl;
  return 0;
}