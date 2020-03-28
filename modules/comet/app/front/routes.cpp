#include <comet/router.hpp>
#include "controllers/application_controller.hpp"

using namespace Comet;

void Router::initialize()
{
  match_route("/?", ApplicationController, home);
}
