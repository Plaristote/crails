#include "crails/server.hpp"
#include "crails/params.hpp"

using namespace std;
using namespace Crails;

void RequestDataParser::operator()(Connection& connection, BuildingResponse&, Params& params, function<void(RequestParser::Status)> callback)
{
  {
    const auto&  request    = connection.get_request();
    const string destination(request.target());
    const char*  get_params = strrchr(destination.c_str(), '?');
    std::string  uri        = destination;

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
  
    // Set URI and method for the posterity (is that even a word ?)
    params["uri"]    = uri;
    params["method"] = boost::beast::http::to_string(request.method());
  }
  callback(RequestParser::Continue);
}
