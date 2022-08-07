#include "crails/request.hpp"
#include "crails/logger.hpp"

using namespace Crails;
using namespace std;

Request::Request(const Server* server, Connection& connection)
  : server(*server), connection(connection), out(connection)
{
}

Request::~Request()
{
}

void Request::operator()()
{
  run_parser(
    server.request_parsers.begin(),
    bind(&Request::on_parsed, this, placeholders::_1)
  );
}

void Request::run_parser(Server::RequestParsers::const_iterator it, function<void(bool)> callback)
{
  server.exception_catcher.run(*this, [=]()
  {
    if (it == server.request_parsers.end())
      callback(server.request_parsers.size() > 0);
    else
    {
      (**it)(connection, out, params, [this, it, callback](RequestParser::Status status)
      {
        if (status == RequestParser::Abort)
          callback(false);
        else if (status == RequestParser::Stop)
          callback(true);
        else
          run_parser(it + 1, callback);
      });
    }
  });
}

void Request::on_parsed(bool parsed)
{
  if (parsed)
  {
    auto callback = std::bind(&Request::on_handled, this, std::placeholders::_1);

    run_handler(server.request_handlers.begin(), callback);
  }
  else
  {
    logger << Logger::Info << "# Request could not be parsed" << Logger::endl;
    params["response-data"]["code"] = (int)(Server::HttpCodes::bad_request);
    on_handled(false);
  }
}

void Request::run_handler(Server::RequestHandlers::const_iterator it, std::function<void(bool)> callback)
{
  server.exception_catcher.run(*this, [=]()
  {
    if (it == server.request_handlers.end())
      callback(false);
    else
    {
      (**it)(connection, out, params, [this, it, callback](bool request_handled)
      {
        if (request_handled)
          callback(true);
        else
          run_handler(it + 1, callback);
      });
    }
  });
}

void Request::on_handled(bool handled)
{
  if (!handled)
  {
    auto code = boost::beast::http::status::not_found;

    if (params["response-data"]["code"].exists())
      code = (Server::HttpCode)(params["response-data"]["code"].as<int>());
    Server::ResponseHttpError(out, code, params);
  }
  on_finished();
}

void Request::on_finished()
{
  float crails_time     = timer.GetElapsedSeconds();
  unsigned short code   = params["response-data"]["code"].defaults_to(200);

  logger << Logger::Info << "# Responded to " << params["method"].defaults_to<string>("GET") << " '" << params["uri"].defaults_to<string>("");
  logger << "' with " << code;
  logger << " in " << crails_time << 's';
  if (params["response-time"]["controller"].exists())
    logger << " (controller: " << params["response-time"]["controller"].as<float>() << "s)";
  logger << Logger::endl << Logger::endl;
}
