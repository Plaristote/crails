#include "crails/appcontroller.hpp"
#include "crails/router.hpp"
#include "crails/backtrace.hpp"

using namespace std;

std::string rand_str(std::string::size_type size); // Defined in rand_str.cpp

ControllerBase::ControllerBase(Params& params) : params(params)
{
  if (must_protect_from_forgery())
    protect_from_forgery();

  // Set the class attributes accessible to the views
  *vars["controller"]       = this;
  *vars["params"]           = &params;
  *vars["flash"]            = &flash;

  // Initialize flash variable and reset corresponding cookie
  flash.Duplicate(params.Session()["flash"]);
  params.Session()["flash"].CutBranch();
  params.Session()["flash"] = "";
}

void ControllerBase::RedirectTo(const string& uri)
{
  Router::Exception302 redirector;
  
  redirector.redirect_to = uri;
  throw redirector;
}

void ControllerBase::protect_from_forgery(void) const
{
  // If request contains a body (ie: not get), check for the CSRF token
  if (params["method"].Value() != "GET" && !(check_csrf_token()))
    throw ExceptionCSRF();
  // Generate a new CSRF token for this request
  params.Session()["csrf_token"] = rand_str(16);
}

bool ControllerBase::check_csrf_token(void) const
{
  Data csrf_token = params["csrf-token"];
  
  if (csrf_token.Nil())
    return (false); // CSRF Token wasn't specified in the request
  return (csrf_token.Value() == params.Session()["csrf_token"].Value());
}

void ControllerBase::render(const std::string& view, const std::string& layout)
{
  Regex      match_ecpp("[.][^.]*[.]ecpp$");
  Regex      match_extension("[.][^.]*$");
  regmatch_t match;
  string     format, body;

  if (!(match_ecpp.Match(view, &match, 1)))
  {
    if (layout != "")
    {
      format = view.substr(match.rm_so + 1, match.rm_eo - match.rm_so - 6);
      body   = View::Render(layout + '.' + format + ".ecpp", view, vars);
    }
    else
    {
      cout << "calling render view for " << view << endl;
      body   = render_view(view, vars);
    }
  }
  else if (!(match_extension.Match(view, &match, 1)))
  {
    format = view.substr(match.rm_so + 1, match.rm_eo - match.rm_so - 1);
    Filesystem::FileContent("../app/views/" + view, body);
  }
  else
    throw boost_ext::invalid_argument("Could not detect format for view `" + view + '`');
  response["body"]                    = body;
  set_content_type_from_extension(format);
}

void ControllerBase::render(RenderType type, const string& value)
{
  string body;

  response["body"] = value;
  set_content_type(type);
}

void ControllerBase::render(RenderType type, DynStruct value)
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

void ControllerBase::set_content_type_from_extension(const std::string& extension)
{
  string content_type;

  if (extension == "html")
    content_type = "text/html";
  else if (extension == "css")
    content_type = "text/css";
  else
  response["headers"]["Content-Type"] = content_type;
}

void ControllerBase::set_content_type(RenderType type)
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
  }
  response["headers"]["Content-Type"] = content_type;
}
