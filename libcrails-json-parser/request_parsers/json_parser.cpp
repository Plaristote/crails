#include <boost/property_tree/json_parser.hpp>
#include "json_parser.hpp"
#include "params.hpp"

using namespace std;
using namespace Crails;
using namespace boost::property_tree;

void RequestJsonParser::operator()(Connection& connection, BuildingResponse& out, Params& params, function<void(RequestParser::Status)> callback) const
{
  static const regex is_json("application/json", regex_constants::extended | regex_constants::optimize);

  if (params["method"].as<string>() != "GET" && content_type_matches(params, is_json))
  {
    wait_for_body(connection, out, params, [callback]()
    {
      callback(RequestParser::Stop);
    });
  }
  else
    callback(RequestParser::Continue);
}

void RequestJsonParser::body_received(Connection&, BuildingResponse&, Params& params, const string& body) const
{
  if (body.size() > 0)
    params.as_data().merge(DataTree().from_json(body));
}
