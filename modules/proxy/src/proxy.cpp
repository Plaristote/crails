#include "crails/request_parsers/proxy.hpp"
#include "crails/logger.hpp"
#include "crails/http_response.hpp"
#include "crails/params.hpp"

using namespace std;
using namespace Crails;

thread_local http::client ProxyRequestHandler::client;

ProxyRequestHandler::ProxyRequestHandler()
{
  DataTree config;

  config.from_json_file("config/proxy.json");
  default_mode = Proxy;
  default_mode = get_mode_from_data(config["default_mode"]);
  config["rules"].each([this](Data rule_data)
  {
    Rule rule;

    rule.matcher     = rule_data["matcher"].as<string>();
    rule.target_url  = rule_data["to"].as<string>();
    rule.target_port = rule_data["to_port"].defaults_to(80);
    rule.mode        = get_mode_from_data(rule_data["mode"]);
    rules.push_back(rule);
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

RequestParser::Status ProxyRequestHandler::operator()(const HttpServer::request& request, BuildingResponse& out, Params& params)
{
  auto rule = find(rules.begin(), rules.end(), request.destination);

  if (rule != rules.end())
  {
    if (request.method != "GET" && request.method != "HEAD")
      wait_for_body(request, out, params);
    else
      body_received(request, out, params, "");
    return RequestParser::Abort;
  }
  return RequestParser::Continue;
}

void ProxyRequestHandler::body_received(const HttpServer::request& request, BuildingResponse& out, Params& params, const string& body)
{
  auto rule = find(rules.begin(), rules.end(), request.destination);

  params["uri"] = request.destination;
  if (rule->mode == Redirect302)
  {
    out.set_headers("Location", get_proxyfied_url(*rule, request.destination));
    out.set_response((ServerTraits::HttpCode)302, "");
    params["response-data"]["code"] = 302;
  }
  else
    execute_rule(*rule, request, body, out);
}

string ProxyRequestHandler::get_proxyfied_url(const Rule& rule, const string& uri)
{
  stringstream result;
  size_t       pos = uri.find(rule.matcher);

  result << rule.target_url << ':' << rule.target_port;
  result << uri.substr(pos + rule.matcher.size(), uri.size());
  return result.str();
}

void ProxyRequestHandler::execute_rule(const Rule& rule, const HttpServer::request& request, const string& body_, BuildingResponse& out)
{
  string                url = get_proxyfied_url(rule, request.destination);
  http::client::request proxy_request(url);

  logger << Logger::Info << "[PROXY] proxyfing `" << request.destination << "` to `" << url << '`' << Logger::endl;
  proxy_request << boost::network::header("Connection", "close");
  for (auto header_ : request.headers)
    proxy_request << boost::network::header(header_.name, header_.value);
  if (request.method != "GET" && request.method != "HEAD")
    proxy_request << boost::network::body(body_);
  try
  {
    http::client::response remote_response;

    if (request.method == "GET")
      remote_response = client.get(proxy_request);
    else if (request.method == "POST")
      remote_response = client.post(proxy_request);
    else if (request.method == "PUT")
      remote_response = client.put(proxy_request);
    else if (request.method == "DELETE")
      remote_response = client.delete_(proxy_request);
    else if (request.method == "HEAD")
      remote_response = client.head(proxy_request);

    for (auto header : remote_response.headers())
      out.set_headers(header.first, header.second);
    out.set_response((Server::HttpCode)((int)status(remote_response)), remote_response.body());
  }
  catch (...)
  {
    out.set_response((Server::HttpCode)(500), "");
  }
}
