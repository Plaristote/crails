#include <crails/request_parsers/proxy.hpp>

using namespace Crails;

/*
 * Modes:
 *  - Redirect302 will issue a temporary redirection response towards the specified URL.
 *  - Proxy will have the query performed by the server. The response will be sent back as is to the client.
 * The default_mode will be used for explicit rules which
 */

const ProxyRequestHandler::Mode ProxyRequestHandler::default_mode = Redirect302;

/*
 * The proxy rules are used to filter which queries are relevant to the proxy.
 * - Each rule starts with a regexp used to match the local actions which will be proxified.
 * - When a regexp doesn't match the entire URI, anything following the pattern will get appended
 *   after the redirect URL. For instance, with our first rule, if a user types "/proxy/redirect/extra",
 *   they will be redirected towards "http://duckduckgo.com/extra".
 * - Instead of specifying a target URL, you may also implement your own handler as a function
 *   that takes the original request and the regexp matches as parameteres. You will then need to
 *   return a ProxyRequest object, describing the request that the Proxy must run before returning its
 *   response to the client.
 */

const ProxyRequestHandler::Rules ProxyRequestHandler::rules = {
  // {"^/proxy/redirect",   "http://duckduckgo.com", ProxyRequestHandler::Redirect302},
  // {"^/proxy",            "http://duckduckgo.com", ProxyRequestHandler::Proxy},
  // {"^/proxy_no_suffix$", "http://duckduckgo.com/?q=Hello%20World"},
  // {"^/custom_proxy",     [](const HttpRequest& request, const std::smatch matches) {
  //     return ProxyRequest(boost::beast::http::verb::get, "http://duckduckgo.com", 80, matches.suffix());
  // }}
};
