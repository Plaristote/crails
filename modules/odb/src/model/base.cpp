#include "../../crails/odb/model/base.hpp"

using namespace std;

std::string ODB::ModelBase::get_database_name() const
{
  return "default";
}

void ODB::ModelBase::save(odb::database& db)
{
  if (id == 0)
    odb_persist(db);
  else
    odb_update(db);
}

void ODB::ModelBase::destroy(odb::database& db)
{
  if (id != 0)
    odb_erase(db);
  erased = true;
}
