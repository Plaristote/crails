#include "../crails/odb/controller.hpp"

using namespace Crails;

ODB::Controller::Controller(Crails::Context& context) : Crails::Controller(context)
{
}

void ODB::Controller::finalize()
{
  if (response.get_status_code() == HttpStatus::ok)
    database.commit();
}
