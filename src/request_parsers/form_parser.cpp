#include "crails/server.hpp"
#include "crails/params.hpp"
#include "crails/logger.hpp"
#include "crails/utils/regex.hpp"

using namespace std;
using namespace Crails;

void RequestFormParser::operator()(const HttpServer::request& request, BuildingResponse& out, Params& params, function<void(RequestParser::Status)> callback)
{
  static const Regex is_form("application/x-www-form-urlencoded", REG_EXTENDED);

  if (params["method"].as<string>() != "GET" && content_type_matches(params, is_form))
  {
    wait_for_body(request, out, params, [callback]()
    {
      callback(RequestParser::Stop);
    });
  }
  else
    callback(RequestParser::Continue);
}

void RequestFormParser::body_received(const HttpServer::request& request, BuildingResponse&, Params& params, const string& body)
{
  if (body.size() > 0)
    cgi2params(params.as_data(), body);
}
