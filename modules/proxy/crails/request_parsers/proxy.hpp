#ifndef  PROXY_REQUEST_HANDLER_HPP
# define PROXY_REQUEST_HANDLER_HPP

# include "crails/request_parser.hpp"
# include <regex>

namespace Crails
{
  class ProxyRequestHandler : public BodyParser
  {
  public:
    enum Mode
    {
      Redirect302,
      Proxy
    };

    struct ProxyRequest : public HttpRequest
    {
      ProxyRequest() {}
      ProxyRequest(boost::beast::http::verb method, const std::string& host, unsigned short port, const std::string& target, unsigned int http_version = 11) : HttpRequest{method, target, http_version}, host(host), port(port) {}
      ProxyRequest(boost::beast::http::verb method, const std::string& host, std::string& target) : HttpRequest{method, target, 11}, host(host), port(80) {}

      bool ssl;
      std::string host;
      unsigned short port;
    };

    typedef std::function<ProxyRequest (const HttpRequest&, const std::smatch&)> RuleSolver;

    struct Rule
    {
      Rule(const char* regex, const char* target, Mode = ProxyRequestHandler::default_mode);
      Rule(const char* regex, RuleSolver solver);

      bool operator==(const std::string& uri) const { return solver && std::regex_search(uri.c_str(), matcher); }
      ProxyRequest operator()(const HttpRequest& source) const;
      static ProxyRequest defaultSolver(ProxyRequest base, const HttpRequest& source, const std::smatch&);

      std::regex matcher;
      Mode       mode;
      RuleSolver solver;
    };

    typedef std::vector<Rule> Rules;
    friend struct Rule;

    ProxyRequestHandler();

    void operator()(Connection&, BuildingResponse& out, Params& params, std::function<void(RequestParser::Status)>) const override;

  private:
    void body_received(Connection&, BuildingResponse&, Params&, const std::string&) const override {}
    void execute_rule(const Rule&, Connection&, BuildingResponse&, std::function<void()> callback) const;
    void proxy(const Rule&, Connection&, BuildingResponse&, std::function<void()> callback) const;
    static std::string get_proxyfied_url(const ProxyRequest&);

    static const Mode  default_mode;
    static const Rules rules;
  };
}

#endif
