#include "crails/request_handlers/action.hpp"
#include "crails/router.hpp"
#include "crails/logger.hpp"

using namespace std;
using namespace Crails;

void ActionRequestHandler::operator()(const HttpServer::request& request, BuildingResponse& out, Params& params, function<void(bool)> callback)
{
  const Router* router = Router::singleton::Get();

  if (router)
  {
    string                method = params["_method"].defaults_to<string>(request.method);
    const Router::Action* action = router->get_action(method, params["uri"].as<string>(), params);

    if (action == 0)
      callback(false);
    else
    {
      logger << Logger::Info << "# Responding to " << method << ' ' << params["uri"].as<string>() << Logger::endl;
      params.session->load(params["headers"]);
      (*action)(params, [callback,&params,&out](DataTree data)
      {
        string           body = data["body"].defaults_to<string>("");
        Server::HttpCode code;

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
        callback(true);
      });
    }
  }
  else
    callback(false);
}

