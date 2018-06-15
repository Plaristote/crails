#include "../crails/odb/transaction.hpp"
#include "../crails/odb/database_settings.hpp"
#include <odb/transaction.hxx>
#include <odb/session.hxx>
#include <odb/pgsql/database.hxx>
#include <crails/databases.hpp>
#include <crails/odb/database.hpp>
#include <thread>

using namespace std;
using namespace Crails;

extern const std::string default_configuration_name;

ODB::Transaction::Transaction() : odb_database(0)
{
}

ODB::Transaction::~Transaction()
{
}

odb::database& ODB::Transaction::get_database()
{
  if (!odb_database)
    throw std::runtime_error("asked for a database on an uninitialized Transaction");
  return *odb_database;
}

void ODB::Transaction::require(const std::string& name)
{
  if (database_name != name || !odb_database)
  {
    if (name == "default")
      start(name, CRAILS_DATABASE(ODB, default_configuration_name).get_agnostic_database());
    else
    {
      auto  database_settings = ODB::get_database_settings_for(name);
      auto& crails_database = CRAILS_DATABASE_FROM_SETTINGS(ODB, name, database_settings);

      start(name, crails_database.get_agnostic_database());
    }
  }
}

void ODB::Transaction::start(const std::string& name, odb::database& database)
{
  rollback();
  try
  {
    database_name   = name;
    odb_transaction = unique_ptr<odb::transaction>(
      new odb::transaction(database.begin())
    );
    if (use_session)
      odb_session   = unique_ptr<odb::session>(new odb::session);
    odb_database    = &database;
  }
  catch (const odb::exception& e)
  {
    throw boost_ext::runtime_error(e.what());
  }
}

void ODB::Transaction::commit()
{
  if (odb_transaction)
  {
    //std::cout << "odb commit" << std::endl;
    try
    {
      odb_transaction->commit();
      cleanup();
    }
    catch (const odb::exception& e)
    {
      throw boost_ext::runtime_error(e.what());
    }
  }
}

void ODB::Transaction::rollback()
{
  if (odb_transaction)
  {
    //std::cout << "odb rollback" << std::endl;
    try
    {
      odb_transaction->rollback();
      cleanup();
    }
    catch (const odb::exception& e)
    {
      logger << Logger::Error << "ODB MAJOR FAILURE: rollback failed: " << e.what() << Logger::endl;
    }
  }
}

void ODB::Transaction::cleanup()
{
  odb_database    = nullptr;
  odb_session     = nullptr;
  odb_transaction = nullptr;
}
