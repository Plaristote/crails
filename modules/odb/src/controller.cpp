#include "../crails/odb/controller.hpp"

using namespace Crails;

Odb::Controller::Controller(Crails::Params& params) : Crails::Controller(params)
{
}

void Odb::Controller::finalize()
{
  if (!(response["status"].exists()) || (response["status"].as<unsigned short>() == Server::HttpCodes::ok))
    database.commit();
}
