#include <boost/property_tree/json_parser.hpp>
#include <crails/server.hpp>
#include <crails/params.hpp>

using namespace std;
using namespace Crails;
using namespace boost::property_tree;

void RequestJsonParser::operator()(const HttpServer::request& request, BuildingResponse& out, Params& params, function<void(RequestParser::Status)> callback)
{
  static const regex is_json("application/json", regex_constants::extended);

  if (params["method"].as<string>() != "GET" && content_type_matches(params, is_json))
  {
    wait_for_body(request, out, params, [callback]()
    {
      callback(RequestParser::Stop);
    });
  }
  else
    callback(RequestParser::Continue);
}

void RequestJsonParser::body_received(const HttpServer::request& request, BuildingResponse&, Params& params, const string& body)
{
  if (body.size() > 0)
    params.as_data().merge(DataTree().from_json(body));
}
