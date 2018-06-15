#include "../../crails/odb/model/soft_delete.hpp"

using namespace std;

void ODB::ModelSoftDelete::save(odb::database& db)
{
  deleted = false;
  BaseType::save(db);
}

void ODB::ModelSoftDelete::destroy(odb::database& db)
{
  if (with_soft_delete() && get_id() != 0)
  {
    deleted = true;
    BaseType::save(db);
  }
  else
    BaseType::destroy(db);
}
