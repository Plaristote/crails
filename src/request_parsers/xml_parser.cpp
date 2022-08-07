#include <boost/property_tree/xml_parser.hpp>
#include <crails/server.hpp>
#include <crails/params.hpp>

using namespace std;
using namespace Crails;
using namespace boost::property_tree;

void RequestXmlParser::operator()(Connection& connection, BuildingResponse& out, Params& params, function<void(RequestParser::Status)> callback)
{
  static const regex is_xml("(application|text)/xml", regex_constants::extended);

  if (params["method"].as<string>() != "GET" && content_type_matches(params, is_xml))
  {
    wait_for_body(connection, out, params, [callback]()
    {
      callback(RequestParser::Stop);
    });
  }
  else
    callback(RequestParser::Continue);
}

void RequestXmlParser::body_received(Connection&, BuildingResponse&, Params& params, const string& body)
{
  if (body.size() > 0)
  {
    params["document"] = 0;
    params["document"].merge(DataTree().from_xml(body));
  }
}

