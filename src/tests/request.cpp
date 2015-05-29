#include "crails/tests/request.hpp"

using namespace Crails;
using namespace Crails::Tests;

Request::Request(const std::string& method, const std::string& uri)
{
  params["method"] = method;
  params["uri"]    = uri;
}

void Request::run()
{
  const Router* router = Router::singleton::Get();

  if (router)
  {
    const Router::Action* action = router->get_action(params["method"].Value(), params["uri"].Value(), params);

    if (action != 0)
      response.Duplicate((*action)(params));
    else
      throw RouteNotFound(params["method"].Value() + '#' + params["uri"].Value());
    if (response["status"].Nil())
      response["status"] = 200;
  }
  else
    throw RouterNotInitialized();
}
