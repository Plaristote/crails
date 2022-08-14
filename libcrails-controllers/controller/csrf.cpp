#include "csrf.hpp"
#include "params.hpp"
#include "utils/string.hpp"

using namespace std;
using namespace Crails;

const char* ExceptionCSRF::what() const throw()
{
  return "The CSRF token was missing or invalid and your identity couldn't be verified.";
}

CsrfController::CsrfController(Context& context) : BasicAuthenticationController(context)
{
}

void CsrfController::initialize()
{
  if (must_protect_from_forgery())
    protect_from_forgery();
}

void CsrfController::protect_from_forgery()
{
  // If request contains a body (ie: not get), check for the CSRF token
  if (params["method"].as<string>() != "GET" && !(check_csrf_token()))
    throw ExceptionCSRF();
  // Generate a new CSRF token for this request
  session["csrf_token"] = generate_random_string(16);
}

bool CsrfController::check_csrf_token()// const
{
  Data csrf_token = params["csrf-token"];

  if (!(csrf_token.exists()))
    return false; // CSRF Token wasn't specified in the request
  return csrf_token.as<string>() == session["csrf_token"].as<string>();
}
