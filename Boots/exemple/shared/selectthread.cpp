#include "selectthread.hpp"

void SelectThread::Run(void)
{
  try
  {
    Network::Select::Run();
  } catch (Exception<Network::Select> exception)
  {
    std::cout << exception.what() << std::endl;
  }
}
