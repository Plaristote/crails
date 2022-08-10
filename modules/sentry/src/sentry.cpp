#include <boost/lexical_cast.hpp>
#include <chrono>
#include "crails/logger.hpp"
#include "crails/server.hpp"
#include "crails/sentry.hpp"

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>
static std::string demangle(const char* name)
{
  int status = -4;
  std::unique_ptr<char, void(*)(void*)> res {
    abi::__cxa_demangle(name, NULL, NULL, &status),
    std::free
  };
  return !status ? res.get() : name;
}
#else
static std::string demangle(const char* name) { return name; }
#endif

using namespace Crails;
using namespace std;
using namespace boost;

template<class T>
string get_type_as_string(const T& t)
{
  return demangle(typeid(t).name());
}

string Sentry::get_server_url()
{
  return server_protocol + "://" + server_url + "/api/" + project_id + "/store/";
}

Sentry::Sentry() :
  url(Url::from_string(get_server_url())),
  client(url.host, url.port)
{
}

void Sentry::set_message_context(Data message)
{
  message["platform"] = "c++";
  message["logger"]   = "crails";
  message["project"]  = project_id;
}

void Sentry::set_message_request(Data message, Data params)
{
  Data request_data = message["request"];
  Data user_agent   = params["headers"]["User-Agent"];

  if (params["headers"].exists())
  {
    request_data["headers"] = "";
    request_data["headers"].merge(params["headers"]);
    request_data["headers"]["Cookie"] = "";
    if (request_data["headers"]["Cookie"].exists())
      request_data["headers"]["Cookie"].destroy();
  }

  Data data = request_data["data"];
  vector<string> ignored_keys = {
    "headers", "uri", "method", "response-data", "response-time", "controller-data"
  };

  data = "";
  data.merge(params);
  for (auto key : ignored_keys)
    data[key].destroy();

  string url = params["headers"]["Host"].defaults_to<string>("");
  if (url.size() > 0) { url = "http://" + url; }
  url += params["uri"].defaults_to<string>("");
  request_data["url"] = url;
  request_data["method"] = params["method"].defaults_to<string>("");
}

void Sentry::initialize_exception_message(Data message, Data params, const std::exception& e)
{
  message["culprit"] = "<anonymous>";
  message["message"] = e.what();
  set_message_context(message);
  set_message_request(message, params);
}

void Sentry::initialize_backtrace(Data exception_data, const boost_ext::backtrace& e)
{
  Data frames = exception_data["stacktrace"]["frames"];

  for (unsigned short i = 3 ; i < e.stack_size() ; ++i)
  {
    DataTree frame;
    string trace_line = e.trace_line(i);

    size_t func_name_beg = trace_line.find(":") + 2;
    size_t func_name_end = trace_line.find("+");
    size_t file_name_beg = trace_line.find(" in ") + 4;

    frame["in_app"]   = trace_line.find("libcrails-app.so") != string::npos;
    frame["filename"] = trace_line.substr(file_name_beg);
    frame["function"] = trace_line.substr(func_name_beg, func_name_end - func_name_beg);
    frames.push_back(frame.as_data());
  }
}

void Sentry::capture_exception(Data params, const std::exception& e)
{
  try
  {
    DataTree message;

    initialize_exception_message(message.as_data(), params, e);
    {
      Data exceptions_data = message["exception"];
      DataTree exception_data;

      exception_data["type"] = get_type_as_string(e);
      exception_data["value"] = e.what();
      {
        boost_ext::backtrace const *tr = dynamic_cast<boost_ext::backtrace const*>(&e);

        if (tr)
          initialize_backtrace(exception_data, *tr);
      }
      exceptions_data.push_back(exception_data.as_data());
    }
    send_message(message.as_data());
  }
  catch (const std::exception& e)
  {
    logger << Logger::Info << "[Sentry] Failed to log exception to sentry server: " << e.what() << Logger::endl;
  }
  catch (...)
  {
    logger << Logger::Info << "[Sentry] Failed to log exception to sentry server." << Logger::endl;
  }
}

void Sentry::send_message(Data message)
{
  string body_str = message.to_json();
  string sentry_auth;
  time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  auto monkey_patch_body = [](string b, string val) -> string
  {
    string with_quote = '"' + val + '"';
    string::size_type n = 0;

    while ((n = b.find(with_quote, n)) != string::npos)
      b.replace(n, with_quote.size(), val);
    return b;
  };

  body_str = monkey_patch_body(body_str, "true");
  body_str = monkey_patch_body(body_str, "false");

  sentry_auth = string("Sentry ")
    + "sentry_version=7,"
    + "sentry_client=sentry_crails,"
    + "sentry_sentry_timestamp" + boost::lexical_cast<string>(timestamp) + ','
    + "sentry_key=" + sentry_key + ','
    + "sentry_secret=" + sentry_secret;

  HttpRequest request{HttpVerb::post, url.target, 11};
  HttpResponse response;

  request.set(HttpHeader::connection,   "close");
  request.set(HttpHeader::content_type, "application/json");
  request.set("X-Sentry-Auth",          sentry_auth);
  request.content_length(body_str.length());
  request.body() = body_str;
  response = client.query(request);
  logger << Logger::Info << "[Sentry] ";
  if (response.result() == HttpStatus::ok)
    logger << "exception logged with id " << response.body() << Logger::endl;
  else 
    logger << "failed to log exception (status " << static_cast<int>(response.result()) << "): " << response.body() << Logger::endl;
}
