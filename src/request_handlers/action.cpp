#include "crails/request_handlers/action.hpp"
#include "crails/router.hpp"
#include "crails/logger.hpp"
#include "crails/context.hpp"
#include "crails/logger.hpp"

using namespace std;
using namespace Crails;

void ActionRequestHandler::operator()(Context& context, function<void(bool)> callback) const
{
  const Router* router = Router::singleton::Get();

  if (router)
  {
    Params&               params      = context.params;
    const auto&           request     = context.connection->get_request();
    string                implicit_method(boost::beast::http::to_string(request.method()));
    string                method = params["_method"].defaults_to<string>(implicit_method);
    const Router::Action* action = router->get_action(method, params["uri"].as<string>(), params);

    if (action == 0)
      callback(false);
    else
    {
      logger << Logger::Info << "# Responding to " << method << ' ' << params["uri"].as<string>() << Logger::endl;
      params.session->load(request);
      (*action)(context, [callback, &context]()
      {
        context.params.session->finalize(context.response);
        callback(true);
      });
    }
  }
  else
    callback(false);
}
