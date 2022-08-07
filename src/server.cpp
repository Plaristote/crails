#include <crails/utils/backtrace.hpp>
#include <crails/utils/timer.hpp>
#include <crails/server.hpp>
#include <crails/router.hpp>
#include <crails/http_response.hpp>
#include <crails/session_store.hpp>
#include <crails/renderer.hpp>
#include <crails/params.hpp>
#include <crails/databases.hpp>
#include <iostream>
#include <crails/request_handlers/file.hpp>
#include <crails/logger.hpp>
#include <crails/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio/signal_set.hpp>
#include "crails/request.hpp"

#include "crails/http_server/listener.hpp"
#include "crails/http_server/connection.hpp"

using namespace std;
using namespace Crails;

Crails::FileCache       Server::file_cache;
Server::RequestParsers  Server::request_parsers;
Server::RequestHandlers Server::request_handlers;
shared_ptr<boost::asio::io_service> Server::io_service;
shared_ptr<boost::asio::io_context> Server::io_context;

const string Server::public_path = boost::filesystem::canonical(
  boost::filesystem::current_path().string() + "/public"
).string();

void Server::add_request_handler(RequestHandler* request_handler)
{
  request_handlers.push_back(request_handler);
}

void Server::add_request_parser(RequestParser* request_parser)
{
  request_parsers.push_back(request_parser);
}

void Server::initialize_exception_catcher()
{
  exception_catcher.add_exception_catcher<std::exception&>("std::exception");
}

void Server::SetResponse(Params& params, BuildingResponse& out, Server::HttpCode code, const string& content)
{
  out.set_response(code, content);
  params["response-data"]["code"]   = (int)code;
  params["response-data"]["length"] = content.size();
}

RequestHandler* Server::get_request_handler(const string& name)
{
  RequestHandlers::const_iterator it = request_handlers.begin();

  for (; it != request_handlers.end() ; ++it)
  {
    RequestHandler* request_handler = *it;

    if (request_handler->get_name() == name)
      return request_handler;
  }
  return 0;
}

void Server::ResponseHttpError(BuildingResponse& out, Server::HttpCode code, Params& params)
{
  FileRequestHandler* file_handler = reinterpret_cast<FileRequestHandler*>(get_request_handler("file"));
  std::stringstream file_name;
  std::stringstream view_name;

  file_name << (unsigned int)(code);
  view_name << "errors/" << file_name.str();
  if (file_handler &&
      file_handler->send_file("public/" + file_name.str() + ".html", out, code, 0))
    params["response-data"]["code"] = (int)code;
  else
  {
    Data response = params["response-data"];

    if (Renderer::can_render(view_name.str(), params.as_data()))
    {
      SharedVars vars;
      string     content_type;

      Renderer::render(view_name.str(), params.as_data(), response, vars);
      if (response["headers"]["Content-Type"].exists())
        out.set_headers("Content-Type", response["headers"]["Content-Type"].as<string>());
    }
    Server::SetResponse(params, out, code, response["body"].defaults_to<string>(""));
  }
}

void Server::operator()(const HttpServer::request&, Response)
{
  //auto request = new Request(this, _request, response);

  //(*request)();
}

void Server::on_request_received(Connection& connection)
{
  auto request = std::make_shared<Request>(this, connection);

  (*request)();
}

void Server::log(const char* to_log)
{
  logger << Logger::Error << "[cpp-netlib] " << to_log << Logger::endl;
}

/*
 * Server Initialization
 */
#ifdef USE_SEGVCATCH
# include <segvcatch.h>
void Server::ThrowCrashSegv()
{
  throw Server::Crash("Segmentation Fault");
}
#endif

void Server::Launch(int argc, const char **argv)
{
  logger << Logger::Info << "## Launching the amazing Crails Server ##" << Logger::endl;
  ProgramOptions options(argc, argv);

#ifdef USE_SEGVCATCH
  segvcatch::init_segv(&CrailsServer::ThrowCrashSegv);
#endif

  Router::singleton::Initialize();
  {
    Router* router = Router::singleton::Get();

    logger << Logger::Info << ">> Initializing routes" << Logger::endl;
    router->initialize();
    logger << Logger::Info << ">> Route initialized" << Logger::endl;
  }
  {
    Server server;
    auto   listener = make_shared<Listener>(get_io_context());
    boost::asio::signal_set  signals(get_io_context(), SIGINT, SIGTERM);
    boost::beast::error_code error_code;

    signals.async_wait([](const boost::system::error_code&, int)
    {
      logger << Logger::Info << ">> Crails server will shut down." << Logger::endl;
      logger << ">> Waiting for requests to end..." << Logger::endl;
      get_io_context().stop();
      logger << ">> Done." << Logger::endl;
    });
    if (listener->listen(options.get_endpoint(), error_code))
    {
      listener->set_handler(
        std::bind(&Server::on_request_received, &server, placeholders::_1)
      );
      listener->run();
      get_io_context().run();
    }
    else
      logger << Logger::Error << "!! Could not listen on endpoint." << Logger::endl;
  }
  Router::singleton::Finalize();  
}

Server::~Server()
{
  for_each(request_handlers.begin(), request_handlers.end(), [](RequestHandler* request_handler)
  {
    delete request_handler;
  });
  for_each(request_parsers.begin(), request_parsers.end(), [](RequestParser* request_parser)
  {
    delete request_parser;
  });
}

Server::Server()
{
  auto thread_count = std::thread::hardware_concurrency();

  io_service = make_shared<boost::asio::io_service>();
  io_context = make_shared<boost::asio::io_context>(thread_count);
  initialize_exception_catcher();
  initialize_request_pipe();
}
