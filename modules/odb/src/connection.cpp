#include "../crails/odb/connection.hpp"
#include <crails/logger.hpp>

using namespace std;

thread_local safe_ptr<ODB::Connection> ODB::Connection::instance;

ODB::Connection::Connection()
{
  if (instance)
    throw runtime_error("only one instance of ODB::Connection allowed per thread");
  instance = shared_ptr<ODB::Connection>(this, [](ODB::Connection*) {});
}

ODB::Connection::~Connection()
{
  rollback();
  instance.reset();
}

void ODB::Connection::commit()
{
  Crails::logger << Crails::Logger::Info << "Transaction commit. Database time: " << time << 's' << Crails::Logger::endl;
  Utils::Timer timer;
  transaction.commit();
  Crails::logger << Crails::Logger::Info << "Transaction committed in " << timer.GetElapsedSeconds() << Crails::Logger::endl;
  time = 0.f;
}

void ODB::Connection::rollback()
{
  transaction.rollback();
  time = 0.f;
}
