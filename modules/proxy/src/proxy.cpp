#include "crails/request_parsers/proxy.hpp"
#include "crails/logger.hpp"
#include "crails/http_response.hpp"
#include "crails/params.hpp"
#include "crails/client.hpp"
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include "crails/logger.hpp"

using namespace std;
using namespace Crails;

ProxyRequestHandler::ProxyRequestHandler()
{
}

void ProxyRequestHandler::operator()(Connection& connection, BuildingResponse& out, Params& params, function<void (RequestParser::Status)> callback) const
{
  const auto& request = connection.get_request();
  string destination(request.target());
  auto it = find(rules.cbegin(), rules.cend(), destination);

  if (it != rules.end())
  {
    const Rule& rule = *it;

    params["uri"] = request.target();
    if (request.method() != boost::beast::http::verb::get && request.method() != boost::beast::http::verb::head)
    {
      wait_for_body(connection, out, params, [this, callback, &connection, &out, &rule]()
      {
        execute_rule(rule, connection, out, std::bind(callback, RequestParser::Abort));
      });
    }
    else
      execute_rule(rule, connection, out, std::bind(callback, RequestParser::Abort));
  }
  else
    callback(RequestParser::Continue);
}

void ProxyRequestHandler::execute_rule(const Rule& rule, Connection& connection, BuildingResponse& out, std::function<void()> callback) const
{
  const auto& request = connection.get_request();
  string destination(request.target());

  if (rule.mode == Redirect302)
  {
    auto proxy_request = rule(request);
    out.set_headers("Location", get_proxyfied_url(proxy_request));
    out.set_response(HttpStatus::temporary_redirect, "");
    out.send();
  }
  else
    proxy(rule, connection, out, callback);
}

string ProxyRequestHandler::get_proxyfied_url(const ProxyRequest& request)
{
  stringstream result;

  result << (request.ssl ? "https": "http") << "://";
  result << request.host << ':' << request.port << '/';
  result << request.target();
  return result.str();
}

void ProxyRequestHandler::proxy(const Rule& rule, Connection& connection, BuildingResponse& out, std::function<void()> callback) const
{
  const HttpRequest& request = connection.get_request();
  ProxyRequest       proxy_request = rule(request);
  auto               http_client = make_shared<Client>(proxy_request.host, proxy_request.port);

  try
  {
    logger << "Crails:Proxy proxifying towards " << proxy_request.host << ':' << proxy_request.port << proxy_request.target() << Logger::endl;
    http_client->connect();
    http_client->async_query(proxy_request, [&connection, &out, callback, http_client](const HttpResponse& response)
    {
      connection.get_response() = response;
      out.send();
      callback();
    });
  }
  catch (const std::exception& exception)
  {
    logger << "Crails::Proxy: failed to proxy request towards " << proxy_request.host << ':' << proxy_request.port
           << ": " << exception.what() << Logger::endl;
  }
}
