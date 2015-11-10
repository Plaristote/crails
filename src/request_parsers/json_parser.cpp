#include <boost/property_tree/json_parser.hpp>
#include <crails/server.hpp>
#include <crails/params.hpp>

using namespace std;
using namespace Crails;
using namespace boost::property_tree;

RequestParser::Status RequestJsonParser::operator()(const HttpServer::request& request, ServerTraits::Response response, Params& params)
{
  static const Regex is_json("application/json", REG_EXTENDED);

  if (params["method"].as<string>() != "GET" && content_type_matches(params, is_json))
  {
    wait_for_body(request, response, params);
    return RequestParser::Stop;
  }
  return RequestParser::Continue;
}

void RequestJsonParser::body_received(const HttpServer::request& request, ServerTraits::Response, Params& params, const string& body)
{
  if (body.size() > 0)
    params.as_data().merge(DataTree().from_json(body));
}
