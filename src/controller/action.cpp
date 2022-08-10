#include "crails/controller/action.hpp"
#include "crails/context.hpp"

using namespace std;
using namespace Crails;

ActionController::ActionController(Context& context) :
  params(context.params),
  session(params.get_session()),
  request(context.connection->get_request()),
  response(context.response),
  context(context.shared_from_this())
{
}

ActionController::~ActionController()
{
  if (close_on_deletion)
    close();
}

string ActionController::get_controller_name() const { return params["controller-data"]["name"].as<string>(); }
string ActionController::get_action_name() const     { return params["controller-data"]["action"].as<string>(); }

void ActionController::redirect_to(const string& uri)
{
  response.set_status_code(HttpStatus::temporary_redirect);
  response.set_header(HttpHeader::location, uri);
  close();
}

void ActionController::respond_with(Crails::HttpStatus code)
{
  response.set_status_code(code);
  close();
}

void ActionController::close()
{
  if (callback)
  {
    params["response-time"]["controller"] = timer.GetElapsedSeconds();
    callback();
    callback = std::function<void()>();
  }
}
