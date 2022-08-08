#include "crails/request_parsers/proxy.hpp"
#include "crails/logger.hpp"
#include "crails/http_response.hpp"
#include "crails/params.hpp"
#include <boost/beast/http/verb.hpp>

using namespace std;
using namespace Crails;

thread_local boost::network::http::client ProxyRequestHandler::client;

ProxyRequestHandler::ProxyRequestHandler()
{
  DataTree config;

  config.from_json_file("config/proxy.json");
  default_mode = Proxy;
  default_mode = get_mode_from_data(config["default_mode"]);
  config["rules"].each([this](Data rule_data) -> bool
  {
    Rule rule;

    rule.matcher     = rule_data["matcher"].as<string>();
    rule.target_url  = rule_data["to"].as<string>();
    rule.target_port = rule_data["to_port"].defaults_to(80);
    rule.mode        = get_mode_from_data(rule_data["mode"]);
    rules.push_back(rule);
    return true;
  });
}

ProxyRequestHandler::Mode ProxyRequestHandler::get_mode_from_data(Data mode) const
{
  string mode_str = mode.defaults_to<string>("");

  if (mode_str == "302_redirect")
    return Redirect302;
  else if (mode_str == "proxy")
    return Proxy;
  else
    logger << Logger::Warning << "Unknown proxy mode " << mode_str << Logger::endl;
  return default_mode;
}

void ProxyRequestHandler::operator()(Connection& connection, BuildingResponse& out, Params& params, function<void (RequestParser::Status)> callback)
{
  const auto& request = connection.get_request();
  string destination(request.target());
  auto rule = find(rules.begin(), rules.end(), destination);

  if (rule != rules.end())
  {
    if (request.method() != boost::beast::http::verb::get && request.method() != boost::beast::http::verb::head)
    {
      wait_for_body(connection, out, params, [callback]()
      {
        callback(RequestParser::Abort);
      });
    }
    else
    {
      body_received(connection, out, params, "");
      callback(RequestParser::Abort);
    }
  }
  else
    callback(RequestParser::Continue);
}

void ProxyRequestHandler::body_received(Connection& connection, BuildingResponse& out, Params& params, const string& body)
{
  const auto& request = connection.get_request();
  string destination(request.target());
  auto rule = find(rules.begin(), rules.end(), destination);

  params["uri"] = request.target();
  if (rule->mode == Redirect302)
  {
    out.set_headers("Location", get_proxyfied_url(*rule, destination));
    out.set_response((HttpStatus)302, "");
    params["response-data"]["code"] = 302;
  }
  else
    execute_rule(*rule, connection, body, out);
}

string ProxyRequestHandler::get_proxyfied_url(const Rule& rule, const string& uri)
{
  stringstream result;
  size_t       pos = uri.find(rule.matcher);

  result << rule.target_url << ':' << rule.target_port;
  result << uri.substr(pos + rule.matcher.size(), uri.size());
  return result.str();
}

void ProxyRequestHandler::execute_rule(const Rule& rule, Connection& connection, const string& body_, BuildingResponse& out)
{
  const auto&           request = connection.get_request();
  string                destination(request.target());
  string                url = get_proxyfied_url(rule, destination);
  boost::network::http::client::request proxy_request(url);

  logger << Logger::Info << "[PROXY] proxyfing `" << destination << "` to `" << url << '`' << Logger::endl;
  proxy_request << boost::network::header("Connection", "close");
  for (const auto& header : request.base())
    proxy_request << boost::network::header(header.name_string(), header.value());
  if (request.method() != boost::beast::http::verb::get && request.method() != boost::beast::http::verb::head)
    proxy_request << boost::network::body(body_);
  try
  {
    boost::network::http::client::response remote_response;

    if (request.method() == boost::beast::http::verb::get)
      remote_response = client.get(proxy_request);
    else if (request.method() == boost::beast::http::verb::post)
      remote_response = client.post(proxy_request);
    else if (request.method() == boost::beast::http::verb::put)
      remote_response = client.put(proxy_request);
    else if (request.method() == boost::beast::http::verb::delete_)
      remote_response = client.delete_(proxy_request);
    else if (request.method() == boost::beast::http::verb::head)
      remote_response = client.head(proxy_request);

    for (auto header : remote_response.headers())
      out.set_headers(header.first, header.second);
    int status_code = static_cast<int>(status(remote_response));
    out.set_response(static_cast<HttpStatus>(status_code), remote_response.body());
  }
  catch (...)
  {
    out.set_response(static_cast<HttpStatus>(500), "");
  }
}
