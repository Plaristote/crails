#include "url_parser.hpp"
#include "params.hpp"
#include "url.hpp"

using namespace std;
using namespace Crails;

void RequestUrlParser::operator()(Connection& connection, BuildingResponse&, Params& params, function<void(RequestParser::Status)> callback) const
{
  {
    const auto&  request    = connection.get_request();
    const string destination(request.target());
    const char*  get_params = strrchr(destination.c_str(), '?');
    string       uri = destination;

    // Setting Headers parameters
    {
      for (const auto& header : request.base())
        params["headers"][std::string(header.name_string())] = std::string(header.value());
    }

    // Getting get parameters
    if (get_params != 0)
    {
      std::string str_params(get_params);

      uri.erase(uri.size() - str_params.size());
      str_params.erase(0, 1);
      cgi2params(params.as_data(), str_params);
    }
  
    params["uri"]    = uri;
    params["method"] = boost::beast::http::to_string(request.method());
  }
  callback(RequestParser::Continue);
}
