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

void ProxyRequestHandler::operator()(Connection& connection, BuildingResponse& response, Params& params, function<void (RequestParser::Status)> callback) const
{
  const auto& request = connection.get_request();
  string destination(request.target());
  auto it = find(rules.cbegin(), rules.cend(), destination);

  if (it != rules.end())
  {
    const Rule& rule = *it;

    params["uri"] = request.target();
    if (request.method() != HttpVerb::get && request.method() != HttpVerb::head)
    {
      wait_for_body(connection, response, params, [this, callback, &request, &response, &rule]()
      {
        execute_rule(rule, request, response, std::bind(callback, RequestParser::Abort));
      });
    }
    else
      execute_rule(rule, request, response, std::bind(callback, RequestParser::Abort));
  }
  else
    callback(RequestParser::Continue);
}

void ProxyRequestHandler::execute_rule(const Rule& rule, const HttpRequest& request, BuildingResponse& response, std::function<void()> callback) const
{
  string destination(request.target());

  if (rule.mode == Redirect302)
  {
    auto proxy_request = rule(request);
    response.set_header(HttpHeader::location, get_proxyfied_url(proxy_request));
    response.set_response(HttpStatus::temporary_redirect, "");
    response.send();
    callback();
  }
  else
    proxy(rule, request, response, callback);
}

string ProxyRequestHandler::get_proxyfied_url(const ProxyRequest& request)
{
  stringstream result;

  result << (request.ssl ? "https": "http") << "://";
  result << request.host << ':' << request.port << '/';
  result << request.target();
  return result.str();
}

static void on_proxy_failure(const char* message, BuildingResponse& response, std::function<void()> callback)
{
  logger << "Crails::Proxy: failed to proxy request towards: " << message << Logger::endl;
  response.set_status_code(HttpStatus::internal_server_error);
  response.send();
  callback();

}

void ProxyRequestHandler::proxy(const Rule& rule, const HttpRequest& request, BuildingResponse& response, std::function<void()> callback) const
{
  shared_ptr<ClientInterface> http_client;
  ProxyRequest proxy_request = rule(request);

  if (proxy_request.ssl)
    http_client = make_shared<Ssl::Client>(proxy_request.host, proxy_request.port);
  else
    http_client = make_shared<Client>(proxy_request.host, proxy_request.port);
  try
  {
    logger << "Crails:Proxy proxifying towards " << proxy_request.host << ':' << proxy_request.port << proxy_request.target() << Logger::endl;
    http_client->connect();
    http_client->async_query(proxy_request, [&response, callback, http_client](const HttpResponse& remote_response, boost::beast::error_code ec)
    {
      if (!ec)
      {
        response.get_raw_response() = remote_response;
        response.send();
        callback();
      }
      else
        on_proxy_failure(ec.message().c_str(), response, callback);
      try { http_client->disconnect(); } catch (...) {}
    });
  }
  catch (const std::exception& exception)
  {
    on_proxy_failure(exception.what(), response, callback);
  }
}
