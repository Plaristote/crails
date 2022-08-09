#include "crails/request_parsers/proxy.hpp"
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include "crails/url.hpp"

using namespace std;
using namespace Crails;

ProxyRequestHandler::ProxyRequestHandler::Rule::Rule(const char* regex, const char* url_string, ProxyRequestHandler::Mode mode) :
  matcher(regex, std::regex_constants::optimize),
  mode(mode)
{
  Url url = Url::from_string(url_string);
  ProxyRequest base;

  base.method(boost::beast::http::verb::get);
  base.ssl  = url.ssl;
  base.host = url.host;
  base.port = url.port;
  base.target(url.target);
  solver = std::bind(&Rule::defaultSolver, base, placeholders::_1, placeholders::_2);
}

ProxyRequestHandler::Rule::Rule(const char* regex, ProxyRequestHandler::RuleSolver solver) : matcher(regex), mode(ProxyRequestHandler::Proxy), solver(solver)
{
}

ProxyRequestHandler::ProxyRequest ProxyRequestHandler::Rule::operator()(const HttpRequest& source) const
{
  std::smatch matches;
  std::string uri(source.target());

  std::regex_search(uri, matches, matcher);
  return solver(source, matches);
}

ProxyRequestHandler::ProxyRequest ProxyRequestHandler::Rule::defaultSolver(ProxyRequest result, const HttpRequest& source, const std::smatch& matches)
{
  string base_target(result.target());
  string suffix(matches.suffix());

  for (auto it = source.cbegin() ; it != source.cend() ; ++it)
    result.insert(it->name_string(), it->value());
  if (suffix.length() > 0)
  {
    if (suffix[0] != '/' && suffix[0] != '?')
      suffix = '/' + suffix;
    result.target(base_target + suffix);
  }
  result.body() = source.body();
  return result;
}
