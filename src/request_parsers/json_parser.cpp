#include "crails/server.hpp"
#include <crails/params.hpp>

using namespace std;
using namespace Crails;

RequestParser::Status RequestJsonParser::operator()(const HttpServer::request& request, ServerTraits::Response response, Params& params)
{
  static const Regex is_json("^application/json", REG_EXTENDED);

  if (params["method"].Value() != "GET" && content_type_matches(params, is_json))
  {
    wait_for_body(request, response, params);
    return RequestParser::Stop;
  }
  return RequestParser::Continue;
}

void RequestJsonParser::body_received(const HttpServer::request& request, ServerTraits::Response, Params& params)
{
  if (request.body.size() > 0)
  {
    DataTree* json = DataTree::Factory::StringJSON(request.body);
    
    if (json)
    {
      params.Duplicate(Data(json));
      delete json;
    }
  }
}