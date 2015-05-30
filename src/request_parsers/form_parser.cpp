#include "crails/server.hpp"
#include "crails/params.hpp"
#include "crails/logger.hpp"
#include <Boots/Utils/regex.hpp>

using namespace std;
using namespace Crails;

RequestParser::Status RequestFormParser::operator()(const HttpServer::request& request, ServerTraits::Response response, Params& params)
{
  static const Regex is_form("^application/x-www-form-urlencoded", REG_EXTENDED);
  if (params["method"].Value() != "GET" && content_type_matches(params, is_form))
  {
    wait_for_body(request, response, params);
    return RequestParser::Stop;
  }
  return RequestParser::Continue;
}

void RequestFormParser::body_received(const HttpServer::request& request, ServerTraits::Response, Params& params)
{
  logger << Logger::Info << "[" << request.method << " " << request.destination << "] Going for form-data parsing" << Logger::endl;
  if (request.body.size() > 0)
    cgi2params(params, request.body);
}