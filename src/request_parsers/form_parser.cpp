#include "crails/server.hpp"
#include "crails/params.hpp"
#include "crails/logger.hpp"
#include <Boots/Utils/regex.hpp>

using namespace std;
using namespace Crails;

RequestParser::Status RequestFormParser::operator()(const HttpServer::request& request, ServerTraits::Response response, Params& params)
{
  static const Regex is_form("^application/x-www-form-urlencoded", REG_EXTENDED);
  auto content_type = params["headers"]["Content-Type"].defaults_to<string>("");

  if (params["method"].as<string>() != "GET" &&
      ((content_type == "application/x-www-form-urlencoded") || content_type_matches(params, is_form)))
  {
    wait_for_body(request, response, params);
    return RequestParser::Stop;
  }
  return RequestParser::Continue;
}

void RequestFormParser::body_received(const HttpServer::request& request, ServerTraits::Response, Params& params, const string& body)
{
  logger << Logger::Info << "[" << request.method << " " << request.destination << "] Going for form-data parsing" << Logger::endl;
  if (body.size() > 0)
    cgi2params(params.as_data(), body);
}
