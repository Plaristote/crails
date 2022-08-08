#include "../crails/odb/controller.hpp"

using namespace Crails;

ODB::Controller::Controller(Crails::Params& params) : Crails::Controller(params)
{
}

void ODB::Controller::finalize()
{
  if (!(response["status"].exists()) || (response["status"].as<unsigned short>() == HttpStatus::ok))
    database.commit();
}
