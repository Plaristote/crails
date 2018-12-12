#include <cheerp/client.h>
#include <crails/front/globals.hpp>
#include "controllers/application_controller.hpp"
#include "layouts/application_layout.hpp"

using namespace std;
using namespace Crails::Front;

Router router;

void webMain()
{
  ApplicationController::register_layout("application", make_shared<ApplicationLayout>());

  router.initialize();
}
