#include "crails/request_handlers/action.hpp"
#include "crails/router.hpp"

using namespace std;
using namespace Crails;

bool ActionRequestHandler::operator()(const HttpServer::request& request, BuildingResponse& out, Params& params)
{
  const Router* router = Router::singleton::Get();

  if (router)
  {
    string         method = (params["_method"].exists() ? params["_method"].as<string>() : request.method);
    const Router::Action* action = router->get_action(method, params["uri"].as<string>(), params);

    if (action == 0)
      return false;
    params.session->load(params["header"]);
    {
      DataTree         data   = (*action)(params);
      string           body   = data["body"].as<string>();
      Server::HttpCode code   = Server::HttpCodes::ok;

      out.set_headers("Content-Type", "text/html");
      if (data["headers"].exists())
      {
        data["headers"].each([&out](Data header)
        {
          out.set_headers(header.get_key(), header.as<string>());
        });
      }
      params.session->finalize(out);
      if (data["status"].exists())
        code = (Server::HttpCode)(data["status"].as<int>());
      Server::SetResponse(params, out, code, body);
    }
    return true;
  }
  return false;
}

