#include <crails/front/mvc/route_helpers.hpp>
#include "router.hpp"
#include "controllers/application_controller.hpp"

void Router::initialize()
{
  route("", ApplicationController, home);
  Crails::Front::Router::initialize();
}
