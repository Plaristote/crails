#include "crails/tests/request.hpp"

using namespace std;
using namespace Crails;

Tests::Request::Request(const string& method, const std::string& uri)
{
  params["method"] = method;
  params["uri"]    = uri;
}

void Tests::Request::run()
{
  const Router* router = Router::singleton::Get();

  if (router)
  {
    const Router::Action* action = router->get_action(params["method"].as<string>(), params["uri"].as<string>(), params);

    if (action != 0)
      (*action)(params, [this](DataTree data) { response = data; });
    else
      throw RouteNotFound(params["method"].as<string>() + '#' + params["uri"].as<string>());
    if (!response["status"].exists())
      response["status"] = 200;
  }
  else
    throw RouterNotInitialized();
}
