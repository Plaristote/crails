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
#include <csignal>
#include "crails/request.hpp"

using namespace std;
using namespace Crails;

Server::RequestParsers  Server::request_parsers;
Server::RequestHandlers Server::request_handlers;
shared_ptr<boost::asio::io_service> Server::io_service;

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
    Data         response = params["response-data"];

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

void Server::operator()(const HttpServer::request& _request, Response response)
{
  auto request = new Request(this, _request, response);

  (*request)();
}

void Server::log(const char* to_log)
{
  logger << Logger::Error << "[cpp-netlib] " << to_log << Logger::endl;
}

/*
 * Server Initialization
 */
#include <boost/thread.hpp>

using namespace boost::network::utils;

typedef boost::shared_ptr<thread_pool> thread_pool_ptr;

#ifdef USE_SEGVCATCH
# include <segvcatch.h>
void Server::ThrowCrashSegv()
{
  throw Server::Crash("Segmentation Fault");
}
#endif

std::function<void (void)> shutdown_lambda;
static void shutdown(int) { shutdown_lambda(); }

void Server::Launch(int argc, char **argv)
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
    Server     handler;
    HttpServer server(options.get_server_options(handler));

    signal(SIGINT, &shutdown);
    shutdown_lambda = [&server]()
    {
      logger << Logger::Info << ">> Crails server will shut down." << Logger::endl;
      logger << ">> Waiting for requests to end..." << Logger::endl;
      server.stop();
      logger << ">> Done." << Logger::endl;
    };
    server.run();
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
  io_service = make_shared<boost::asio::io_service>();
  initialize_exception_catcher();
  initialize_request_pipe();
}
