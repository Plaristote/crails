#include "crails/request_handlers/action.hpp"
#include "crails/router.hpp"

using namespace std;
using namespace Crails;

bool ActionRequestHandler::operator()(const HttpServer::request& request, BuildingResponse& out, Params& params)
{
  const Router* router = Router::singleton::Get();

  if (router)
  {
    string                method = params["_method"].defaults_to<string>(request.method);
    const Router::Action* action = router->get_action(method, params["uri"].as<string>(), params);

    if (action == 0)
      return false;
    params.session->load(params["headers"]);
    {
      DataTree         data   = (*action)(params);
      string           body   = data["body"].defaults_to<string>("");
      Server::HttpCode code   = Server::HttpCodes::ok;

      if (data["headers"].exists())
      {
        data["headers"].each([&out](Data header)
        {
          out.set_headers(header.get_key(), header.as<string>());
        });
      }
      params.session->finalize(out);
      code = (Server::HttpCode)(data["status"].defaults_to<int>(200));
      Server::SetResponse(params, out, code, body);
    }
    return true;
  }
  return false;
}

