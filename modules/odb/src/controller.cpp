#include "../crails/odb/controller.hpp"
#include "crails/http_response.hpp"

using namespace Crails;

ODB::Controller::Controller(Crails::Context& context) : Crails::Controller(context)
{
}

void ODB::Controller::finalize()
{
  if (response.sent() && response.get_status_code() == HttpStatus::ok)
    database.commit();
}
