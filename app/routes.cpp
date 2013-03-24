#include <crails/router.hpp>

#include "controllers/main_controller.hpp"
#include "controllers/crm_accounts.hpp"

void Router::Initialize(void)
{
  /*
   * Routes samples
   */
  //SetRoute("GET", "/",                  MyController,          root);
  //SetRoute("GET",  "/crm_accounts",     CrmAccountsController, index);
  //SetRoute("GET",  "/crm_accounts/:id", CrmAccountsController, show);
}
