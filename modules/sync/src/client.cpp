#include "../crails/sync/client.hpp"
#include <crails/server.hpp>
#include <boost/lexical_cast.hpp>

using namespace Faye;
using namespace std;
using namespace boost;

typedef network::http::client_options<network::http::client::tag_type> http_options;

Client::Client(const Settings& settings) :
  settings(settings),
  http(http_options())
{
}

void Client::publish(const string& channel, Data message)
{
  DataTree object;

  object["channel"] = channel;
  object["data"] = message.to_json();
  settings.hook(object.as_data());

  network::http::client::request request(url_for_channel(channel));
  string body = object.to_json();

  request << network::header("Connection", "close");
  request << network::header("Content-Length", lexical_cast<string>(body.length()));
  request << network::header("Content-Type", "application/json");
  request << network::body(body);
  http.post(request);
}

std::string Client::url_for_channel(const string& channel) const
{
  return settings.protocol + "://"
       + settings.hostname + ':'
       + lexical_cast<string>(settings.port);
}
