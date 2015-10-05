#include "crails/controller.hpp"
#include "crails/router.hpp"
#include "crails/renderer.hpp"
#include <crails/logger.hpp>
#include <Boots/Utils/backtrace.hpp>
#include <Boots/Utils/directory.hpp>

using namespace std;
using namespace Crails;

std::string rand_str(std::string::size_type size); // Defined in rand_str.cpp

Controller::Controller(Params& params) : params(params), session(params.get_session())
{
  // Set the class attributes accessible to the views
  *vars["controller"]       = this;
  *vars["params"]           = &params;
  *vars["flash"]            = &flash;

  // Initialize flash variable and reset corresponding cookie
  flash.Duplicate(session["flash"]);
  session["flash"].CutBranch();
  session["flash"] = "";
}

Controller::~Controller()
{
  params["response-time"]["controller"] = timer.GetElapsedSeconds();
}

void Controller::initialize()
{
  if (must_protect_from_forgery())
    protect_from_forgery();
}

void Controller::redirect_to(const string& uri)
{
  response["status"]              = 302;
  response["headers"]["Location"] = uri;
}

void Controller::protect_from_forgery(void) const
{
  // If request contains a body (ie: not get), check for the CSRF token
  if (params["method"].Value() != "GET" && !(check_csrf_token()))
    throw ExceptionCSRF();
  // Generate a new CSRF token for this request
  session["csrf_token"] = rand_str(16);
}

bool Controller::check_csrf_token(void) const
{
  Data csrf_token = params["csrf-token"];
  
  if (csrf_token.Nil())
    return (false); // CSRF Token wasn't specified in the request
  return (csrf_token.Value() == session["csrf_token"].Value());
}

void Controller::render(const std::string& view)
{
  std::string format = params["headers"]["Accept"].Value();

  for (auto it = renderers.begin() ; it != renderers.end() ; ++it)
  {
    Renderer* renderer = *it;

    if (renderer->can_render(format, view))
    {
      renderer->render(view, params, response, vars);
      return ;
    }
  }
}

void Controller::render(RenderType type, const string& value)
{
  string body;

  response["body"] = value;
  set_content_type(type);
}

void Controller::render(RenderType type, DynStruct value)
{
  string       content_type;
  stringstream body;
  
  switch (type)
  {
    case JSON:
    {
      std::string json;

      DataTree::Writers::StringJSON(value, json);
      body << json;
      break ;
    }
    default:
      throw boost_ext::invalid_argument("ControllerBase::render(RenderType,DynStruct) only supports JSON type");
  }
  response["body"] = body.str();
  set_content_type(type);
}

void Controller::set_content_type_from_extension(const std::string& extension)
{
  string content_type;

  if (extension == "html")
    content_type = "text/html";
  else if (extension == "css")
    content_type = "text/css";
  else if (extension == "json")
    content_type = "application/json";
  else if (extension == "js")
    content_type = "application/javascript";
  else
    content_type = "application/octet-stream";
  response["headers"]["Content-Type"] = content_type;
}

void Controller::set_content_type(RenderType type)
{
  string content_type;

  switch (type)
  {
    case JSON:
      content_type = "application/json";
      break ;
    case JSONP:
      content_type = "application/javascript";
      break ;
    case XML:
      content_type = "application/xml";
      break ;
    case HTML:
      content_type = "text/html";
      break ;
    case TEXT:
      content_type = "text/plain";
      break ;
  }
  response["headers"]["Content-Type"] = content_type;
}
