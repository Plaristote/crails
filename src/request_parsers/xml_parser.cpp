#include <boost/property_tree/xml_parser.hpp>
#include <crails/server.hpp>
#include <crails/params.hpp>

using namespace std;
using namespace Crails;
using namespace boost::property_tree;

void RequestXmlParser::operator()(const HttpServer::request& request, BuildingResponse& out, Params& params, function<void(RequestParser::Status)> callback)
{
  static const regex is_xml("(application|text)/xml", regex_constants::extended);

  if (params["method"].as<string>() != "GET" && content_type_matches(params, is_xml))
  {
    wait_for_body(request, out, params, [callback]()
    {
      callback(RequestParser::Stop);
    });
  }
  else
    callback(RequestParser::Continue);
}

void RequestXmlParser::body_received(const HttpServer::request& request, BuildingResponse&, Params& params, const string& body)
{
  if (body.size() > 0)
  {
    params["document"] = 0;
    params["document"].merge(DataTree().from_xml(body));
  }
}

