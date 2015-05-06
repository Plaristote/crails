#include "crails/request_handlers/action.hpp"
#include "crails/router.hpp"

using namespace std;
using namespace Crails;

bool ActionRequestHandler::operator()(const HttpServer::request& request, BuildingResponse& out, Params& params)
{
  const Router* router = Router::singleton::Get();

  if (router)
  {
    string         method = (params["_method"].Nil() ? request.method : params["_method"].Value());
    const Router::Action* action = router->get_action(method, params["uri"].Value(), params);

    if (action == 0)
      return false;
    params.session->Load(params["header"]);
    {
      DynStruct        data   = (*action)(params);
      string           body   = data["body"].Value();
      Server::HttpCode code   = Server::HttpCodes::ok;

      out.SetHeaders("Content-Type", "text/html");
      if (data["headers"].NotNil())
      { // If parameters headers were set, copy them to the response
        auto it  = data["headers"].begin();
        auto end = data["headers"].end();

        for (; it != end ; ++it)
          out.SetHeaders((*it).Key(), (*it).Value());
      }
      params.session->Finalize(out);
      if (data["status"].NotNil())
        code = (Server::HttpCode)((int)data["status"]);
      Server::SetResponse(params, out, code, body);
    }
    return true;
  }
  return false;
}

