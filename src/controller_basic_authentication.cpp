#include "crails/controller.hpp"
#include <crails/utils/string.hpp>

using namespace std;
using namespace Crails;

bool Controller::require_basic_authentication(const string& username, const string& password)
{
  return require_basic_authentication([username, password](const string& a, const string& b) -> bool
  {
    return username == a && password == b;
  });
}

bool Controller::require_basic_authentication(std::function<bool (const std::string&, const std::string&)> acceptor)
{
  Data authorization_header = params["headers"]["Authorization"];

  if (authorization_header.exists())
    return check_basic_authentication_header(authorization_header, acceptor);
  else
  {
    response["status"] = 401;
    response["headers"]["WWW-Authenticate"] = "Basic realm=\"User Visible realm\"";
  }
  return false;
}

bool Controller::check_basic_authentication_header(const string& authorization_header, function<bool (const string&, const string&)> acceptor)
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
        response["status"] = 401;
    }
    else
      response["status"] = 422;
  }
  else
    response["status"] = 422;
  return false;
}
