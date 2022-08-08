#include "crails/request_handlers/action.hpp"
#include "crails/router.hpp"
#include "crails/logger.hpp"
#include "crails/request.hpp"

using namespace std;
using namespace Crails;

void ActionRequestHandler::operator()(Request& request, function<void(bool)> callback)
{
  const Router* router = Router::singleton::Get();

  if (router)
  {
    Params&               params      = request.params;
    const auto&           raw_request = request.connection->get_request();
    string                implicit_method(boost::beast::http::to_string(raw_request.method()));
    string                method = params["_method"].defaults_to<string>(implicit_method);
    const Router::Action* action = router->get_action(method, params["uri"].as<string>(), params);

    if (action == 0)
      callback(false);
    else
    {
      logger << Logger::Info << "# Responding to " << method << ' ' << params["uri"].as<string>() << Logger::endl;
      params.session->load(request.params["headers"]);
      (*action)(params, [callback, &request](DataTree data)
      {
        string     body = data["body"].defaults_to<string>("");
        HttpStatus code;

        if (data["headers"].exists())
        {
          data["headers"].each([&request](Data header) -> bool
          {
            request.out.set_headers(header.get_key(), header.as<string>());
            return true;
          });
        }
        request.params.session->finalize(request.out);
        code = static_cast<HttpStatus>(data["status"].defaults_to<int>(200));
        request.out.set_response(code, body);
        callback(true);
      });
    }
  }
  else
    callback(false);
}

