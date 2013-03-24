#include "include/appcontroller.hpp"
#include "crails/router.hpp"

using namespace std;

void ControllerBase::RedirectTo(const string& uri)
{
  Router::Exception302 redirector;
  
  redirector.redirect_to = uri;
  throw redirector;
}