#include <crails/odb/database_loaders.hpp>
#include <crails/odb/transaction.hpp>

using namespace ODB;

const bool ODB::Transaction::use_session = false;

const Database::Initializers Database::initializers = {
  { sqlite, &initialize_for_sqlite },
  { pgsql,  &initialize_for_postgresql },
  { mysql,  &initialize_for_mysql },
  { oracle, &initialize_for_oracle }
};
