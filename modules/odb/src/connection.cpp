#include "../crails/odb/connection.hpp"
#include <crails/logger.hpp>

using namespace std;

thread_local safe_ptr<Db::Connection> Db::Connection::instance;

Db::Connection::Connection()
{
  if (instance)
    throw runtime_error("only one instance of Db::Connection allowed per thread");
  instance = shared_ptr<Db::Connection>(this, [](Db::Connection*) {});
}

Db::Connection::~Connection()
{
  rollback();
  instance.reset();
}

void Db::Connection::commit()
{
  Crails::logger << Crails::Logger::Info << "Transaction commit. Database time: " << time << 's' << Crails::Logger::endl;
  Utils::Timer timer;
  transaction.commit();
#ifdef WITH_CRAILS_SYNC
  if (use_sync_transaction)
    sync_transaction.commit();
#endif
  Crails::logger << Crails::Logger::Info << "Transaction committed in " << timer.GetElapsedSeconds() << Crails::Logger::endl;
  time = 0.f;
}

void Db::Connection::rollback()
{
  transaction.rollback();
#ifdef WITH_CRAILS_SYNC
  if (use_sync_transaction)
    sync_transaction.rollback();
#endif
  time = 0.f;
}
