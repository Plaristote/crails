#include <crails/sql/model.hpp>
#include <crails/sql/criteria.hpp>

using namespace std;
using namespace SQL;
using namespace soci;

Model::Model(Table table) :
  field_id("id", 0), table(table), initialized(false), row_ptr(0)
{
  add_field(field_id);
}

Model::Model(Table table, soci::row& row) :
  field_id("id", 0), table(table), initialized(true), row_ptr(&row)
{
  add_field(field_id);
}

void Model::add_field(IField& field)
{
  if (row_ptr)
    field.initialize(*row_ptr);
  fields.push_back(&field);
}

void Model::insert(void)
{
  Criteria criteria(table);

  field_id.value = criteria.insert(fields);
  if (field_id.value != 0)
    initialized = true;
}

void Model::update(void)
{
  Criteria criteria(table);

  criteria.where("id", Criteria::EqualTo, Id());
  criteria.update(fields);
}
