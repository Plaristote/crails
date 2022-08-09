#include "crails/controller.hpp"
#include "crails/router.hpp"
#include "crails/renderer.hpp"
#include "crails/request.hpp"
#include <crails/logger.hpp>
#include <crails/utils/backtrace.hpp>

using namespace std;
using namespace Crails;

std::string rand_str(std::string::size_type size); // Defined in rand_str.cpp

Controller::Controller(Request& request) :
  params(request.params),
  session(params.get_session()),
  response(request.out),
  request(request.shared_from_this())
{
  // Set the class attributes accessible to the views
  vars["controller"] = this;
  vars["params"]     = &params;
  vars["flash"]      = &flash;

  // Initialize flash variable and reset corresponding cookie
  if (session["flash"].exists())
  {
    flash.as_data().merge(session["flash"]);
    session["flash"].destroy();
  }
}

Controller::~Controller()
{
  if (close_on_deletion)
    close();
}

void Controller::initialize()
{
  if (must_protect_from_forgery())
    protect_from_forgery();
}

void Controller::close()
{
  params["response-time"]["controller"] = timer.GetElapsedSeconds();
  if (callback)
  {
    callback();
    callback = std::function<void()>();
  }
}

void Controller::redirect_to(const string& uri)
{
  request->out.set_status_code(HttpStatus::temporary_redirect);
  request->out.set_headers("Location", uri);
  close();
}

void Controller::respond_with(Crails::HttpStatus code)
{
  request->out.set_status_code(code);
}

void Controller::protect_from_forgery(void)
{
  // If request contains a body (ie: not get), check for the CSRF token
  if (params["method"].as<string>() != "GET" && !(check_csrf_token()))
    throw ExceptionCSRF();
  // Generate a new CSRF token for this request
  session["csrf_token"] = rand_str(16);
}

bool Controller::check_csrf_token(void)// const
{
  Data csrf_token = params["csrf-token"];
  
  if (!(csrf_token.exists()))
    return (false); // CSRF Token wasn't specified in the request
  return (csrf_token.as<string>() == session["csrf_token"].as<string>());
}

void Controller::render(const std::string& view)
{
  Renderer::render(view, params.as_data(), request->out, vars);
  close();
}

void Controller::render(const std::string& view, SharedVars vars)
{
  for (const auto& var : this->vars)
  {
    if (vars.find(var.first) == vars.end())
      vars.emplace(var.first, var.second);
  }
  Renderer::render(view, params.as_data(), request->out, vars);
  close();
}

void Controller::render(RenderType type, const string& value)
{
  set_content_type(type);
  request->out.set_body(value.c_str(), value.length());
  close();
}

void Controller::render(RenderType type, Data value)
{
  string       content_type;
  stringstream body;
  
  switch (type)
  {
    case JSON:
      value.output(body);
      break ;
    case XML:
      body << value.to_xml();
      break ;
    default:
      throw boost_ext::invalid_argument("Crails::Controller::render(RenderType,Data) only supports JSON and XML");
  }
  set_content_type(type);
  request->out.set_body(body.str().c_str(), body.str().length());
  close();
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
  request->out.set_headers("Content-Type", content_type);
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
      content_type = "text/xml";
      break ;
    case HTML:
      content_type = "text/html";
      break ;
    case TEXT:
      content_type = "text/plain";
      break ;
  }
  request->out.set_headers("Content-Type", content_type);
}
