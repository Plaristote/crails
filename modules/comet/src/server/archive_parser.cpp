#include <crails/archive.hpp>
#include <crails/archive_parser.hpp>
#include "crails/server.hpp"
#include "crails/params.hpp"
#include "crails/logger.hpp"
#include "crails/utils/regex.hpp"

using namespace std;
using namespace Crails;

void RequestArchiveParser::operator()(Connection& connection, BuildingResponse& out, Params& params, function<void(RequestParser::Status)> callback)
{
  static const regex is_form(Archive::mimetype, regex_constants::extended);

  if (params["method"].defaults_to<string>("GET") != "GET" && content_type_matches(params, is_form))
  {
    wait_for_body(connection, out, params, [callback]()
    {
      callback(RequestParser::Stop);
    });
  }
  else
    callback(RequestParser::Continue);
}

void RequestArchiveParser::body_received(Connection&, BuildingResponse&, Params& params, const string& body)
{
  if (body.size() > 0)
    params["archive_data"] = body;
}
