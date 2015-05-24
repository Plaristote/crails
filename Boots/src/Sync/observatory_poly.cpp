#include <Boots/Sync/observatory_poly.hpp>

using namespace Sync;

/*
 * IPolymorphicSignal
 */
IPolymorphicSignal::~IPolymorphicSignal()
{
  if (_master)
    _master->UnlinkSignal(this);
}

/*
 * PolymorphicSignalMaster
 */
PolymorphicSignalMaster& PolymorphicSignalMaster::operator<<(IPolymorphicSignal* signal)
{
  Mutex::Lock lock(_semaphore);

  signal->_master = this;
  _signals.push_back(signal);
  return (*this);
}

PolymorphicSignalMaster::~PolymorphicSignalMaster()
{
  Mutex::Lock lock(_semaphore);

  std::for_each(_signals.begin(), _signals.end(), [this](IPolymorphicSignal* signal)
  {
    signal->_master = 0;
  });
}

void PolymorphicSignalMaster::UnlinkSignal(IPolymorphicSignal* signal)
{
  Mutex::Lock lock(_semaphore);

  signal->_master = 0;
  _signals.remove(signal);
}

void PolymorphicSignalMaster::Refresh(void)
{
  Mutex::Lock lock(_semaphore);

  std::for_each(_signals.begin(), _signals.end(), [](IPolymorphicSignal* signal)
  {
    if (signal->PushedCallCount() > 0)
      std::cout << signal << ':' << signal->GetIdentifier() << " has stuff to refresh" << std::endl;
    signal->Refresh();
  });
}

