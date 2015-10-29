#include "crails/server.hpp"
#include "crails/params.hpp"

using namespace std;
using namespace Crails;

RequestParser::Status RequestDataParser::operator()(const HttpServer::request& request, ServerTraits::Response response, Params& params)
{
  const char*  get_params = strrchr(request.destination.c_str(), '?');
  std::string  uri        = request.destination;

  // Setting Headers parameters
  {
    auto it  = request.headers.begin();
    auto end = request.headers.end();

    for (; it != end ; ++it)
    {
      boost::network::http::request_header_narrow header = *it;
      params["headers"][header.name] = header.value;
    }      
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
  params["method"] = request.method;
  return RequestParser::Continue;
}
