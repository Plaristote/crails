#include "../crails/sync/client.hpp"
#include <crails/server.hpp>
#include <boost/lexical_cast.hpp>

using namespace Faye;
using namespace std;
using namespace boost;

Client::Client(const Settings& settings) :
  settings(settings),
  http(settings.hostname, settings.port)
{
}

void Client::publish(const string& channel, Data message)
{
  DataTree object;
  Crails::HttpRequest request{boost::beast::http::verb::post, channel, 11};
  string body;

  object["channel"] = channel;
  object["data"] = message.to_json();
  settings.hook(object.as_data());
  body = object.to_json();
  request.set(boost::beast::http::field::connection, "Close");
  request.set(boost::beast::http::field::content_type, "application/json");
  request.content_length(body.length());
  request.body() = body;
  http.query(request);
}
