#ifndef  PROXY_REQUEST_HANDLER_HPP
# define PROXY_REQUEST_HANDLER_HPP

# include "crails/request_parser.hpp"
# include <boost/network/protocol/http/client.hpp>

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

    struct Rule
    {
      bool operator==(const std::string& uri) const { return uri.find(matcher) != std::string::npos; }

      std::string    matcher;
      std::string    target_url;
      unsigned short target_port;
      Mode           mode;
    };

    typedef std::vector<Rule> Rules;

    ProxyRequestHandler();

    void operator()(Connection&, BuildingResponse& out, Params& params, std::function<void(RequestParser::Status)>);

  private:
    void body_received(Connection&, BuildingResponse& out, Params& params, const std::string& body);
    void execute_rule(const Rule& rule, Connection&, const std::string& body, BuildingResponse&);
    Mode get_mode_from_data(Data) const;
    static std::string get_proxyfied_url(const Rule& rule, const std::string& uri);

    static thread_local boost::network::http::client client;
    Mode  default_mode;
    Rules rules;
  };
}

#endif
