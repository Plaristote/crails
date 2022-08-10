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
  Crails::HttpRequest request{Crails::HttpVerb::post, channel, 11};
  string body;

  object["channel"] = channel;
  object["data"] = message.to_json();
  settings.hook(object.as_data());
  body = object.to_json();
  request.set(Crails::HttpHeader::connection, "Close");
  request.set(Crails::HttpHeader::content_type, "application/json");
  request.content_length(body.length());
  request.body() = body;
  http.query(request);
}
