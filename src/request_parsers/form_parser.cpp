#include "crails/request_parser.hpp"
#include "crails/params.hpp"
#include "crails/utils/regex.hpp"
#include "crails/helpers.hpp"

using namespace std;
using namespace Crails;

void RequestFormParser::operator()(Connection& connection, BuildingResponse& out, Params& params, function<void(RequestParser::Status)> callback)
{
  static const regex is_form("application/x-www-form-urlencoded", regex_constants::extended);

  if (params["method"].as<string>() != "GET" && content_type_matches(params, is_form))
  {
    wait_for_body(connection, out, params, [callback]()
    {
      callback(RequestParser::Stop);
    });
  }
  else
    callback(RequestParser::Continue);
}

void RequestFormParser::body_received(Connection&, BuildingResponse&, Params& params, const string& body)
{
  if (body.size() > 0)
    cgi2params(params.as_data(), body);
}
