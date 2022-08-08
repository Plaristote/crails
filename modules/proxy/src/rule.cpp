#include "crails/request_parsers/proxy.hpp"
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include "crails/logger.hpp"

using namespace std;
using namespace Crails;

ProxyRequestHandler::ProxyRequestHandler::Rule::Rule(const char* regex, const char* url, ProxyRequestHandler::Mode mode) :
  matcher(regex, std::regex_constants::optimize),
  mode(mode)
{
  static const std::regex url_matcher("^(https?)://([^/:]+)(:([0-9]{1,5}))?/?(.*)$");
  std::smatch matches;
  ProxyRequest base;
  std::string a(url);

  if (std::regex_match(a, matches, url_matcher))
  {
    base.method(boost::beast::http::verb::get);
    base.ssl    = matches[1].str() == "https";
    base.host   = matches[2].str();
    base.port   = matches[4].str().length() > 0 ? boost::lexical_cast<unsigned short>(matches[4].str()) : (base.ssl ? 443 : 80);
    base.target(matches[5].str());
    solver = std::bind(&Rule::defaultSolver, base, placeholders::_1, placeholders::_2);
  }
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
