#include "basic_authentication.hpp"
#include "utils/string.hpp"
#include "params.hpp"
#include "http_response.hpp"

using namespace std;
using namespace Crails;

BasicAuthenticationController::BasicAuthenticationController(Context& context) : ActionController(context)
{
}

bool BasicAuthenticationController::require_basic_authentication(const string& username, const string& password)
{
  return require_basic_authentication([username, password](const string& a, const string& b) -> bool
  {
    return username == a && password == b;
  });
}

bool BasicAuthenticationController::require_basic_authentication(std::function<bool (const std::string&, const std::string&)> acceptor)
{
  Data authorization_header = params["headers"]["Authorization"];

  if (authorization_header.exists())
    return check_basic_authentication_header(authorization_header, acceptor);
  else
  {
    response.set_status_code(HttpStatus::unauthorized);
    response.set_header(boost::beast::http::field::www_authenticate, "Basic realm=\"User Visible realm\"");
  }
  return false;
}

bool BasicAuthenticationController::check_basic_authentication_header(const string& authorization_header, function<bool (const string&, const string&)> acceptor)
{
  auto parts = split(authorization_header, ' ');

  if (parts.size() == 2 && *parts.begin() == "Basic")
  {
    string decoded_credentials = base64_decode(*parts.rbegin());
    auto separator_position = decoded_credentials.find(':');

    if (separator_position != string::npos)
    {
      if (acceptor(decoded_credentials.substr(0, separator_position), decoded_credentials.substr(separator_position + 1)))
        return true;
      else
        response.set_status_code(HttpStatus::unauthorized);
    }
    else
      response.set_status_code(HttpStatus::unprocessable_entity);
  }
  else
    response.set_status_code(HttpStatus::unprocessable_entity);
  return false;
}
