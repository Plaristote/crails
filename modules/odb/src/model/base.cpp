#include "../../crails/odb/model/base.hpp"

using namespace std;

std::string Db::ModelBase::get_database_name() const
{
  return "default";
}

void Db::ModelBase::save(odb::database& db)
{
  if (id == 0)
    odb_persist(db);
  else
    odb_update(db);
}

void Db::ModelBase::destroy(odb::database& db)
{
  if (id != 0)
    odb_erase(db);
  erased = true;
}
