#include "../crails/odb/transaction.hpp"
#include "../crails/odb/database_settings.hpp"
#include <odb/transaction.hxx>
#include <odb/pgsql/database.hxx>
#include <crails/databases.hpp>
#include <crails/odb/database.hpp>
#include <thread>

using namespace std;
using namespace Crails;

extern const std::string default_configuration_name;

Db::Transaction::Transaction() : odb_database(0)
{
}

Db::Transaction::~Transaction()
{
}

odb::database& Db::Transaction::get_database()
{
  if (!odb_database)
    throw std::runtime_error("asked for a database on an uninitialized Transaction");
  return *odb_database;
}

void Db::Transaction::require(const std::string& name)
{
  if (database_name != name || !odb_database)
  {
    if (name == "default")
      start(name, CRAILS_DATABASE(ODB, default_configuration_name).get_agnostic_database());
    else
    {
      auto  database_settings = Db::get_database_settings_for(name);
      auto& crails_database = CRAILS_DATABASE_FROM_SETTINGS(ODB, name, database_settings);

      start(name, crails_database.get_agnostic_database());
    }
  }
}

void Db::Transaction::start(const std::string& name, odb::database& database)
{
  rollback();
  try
  {
    database_name   = name;
    odb_transaction = unique_ptr<odb::transaction>(
      new odb::transaction(database.begin())
    );
    odb_database    = &database;
  }
  catch (const odb::exception& e)
  {
    throw boost_ext::runtime_error(e.what());
  }
}

void Db::Transaction::commit()
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

void Db::Transaction::rollback()
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

void Db::Transaction::cleanup()
{
  odb_database    = nullptr;
  odb_transaction = nullptr;
}
