#include "crails/appcontroller.hpp"
#include "crails/router.hpp"

using namespace std;

std::string rand_str(std::string::size_type size); // Defined in rand_str.cpp

ControllerBase::ControllerBase(Params& params, bool must_protect_from_forgery) : params(params)
{
  if (must_protect_from_forgery)
  {
    // If request contains a body (ie: not get), check for the CSRF token
    if (params["method"].Value() != "GET" && !(protect_from_forgery()))
      throw ExceptionCSRF();
    // Generate a new CSRF token for this request
    params.Session()["csrf_token"] = rand_str(16);
  }

  // Set the class attributes accessible to the views
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

bool ControllerBase::protect_from_forgery()
{
  Data csrf_token = params["csrf-token"];
  
  if (csrf_token.Nil())
    return (false); // CSRF Token wasn't specified in the request
  return (csrf_token.Value() == params.Session()["csrf_token"].Value());
}
