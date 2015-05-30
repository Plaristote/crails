#include <Boots/Utils/directory.hpp>
#include <Boots/Utils/timer.hpp>
#include <crails/server.hpp>
#include <crails/router.hpp>
#include <crails/http_response.hpp>
#include <crails/session_store.hpp>
#include <crails/view.hpp>
#include <crails/params.hpp>
#include <crails/databases.hpp>
#include <crails/backtrace.hpp>
#include <iostream>
#include <crails/request_handlers/file.hpp>
#include <crails/logger.hpp>
#undef log

using namespace std;
using namespace Crails;

Server::RequestParsers  Server::request_parsers;
Server::RequestHandlers Server::request_handlers;

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

  file_name << (unsigned int)(code) << ".html";
  if (file_handler && file_handler->send_file("public/" + file_name.str(), out, code, 0))
  {
    params["response-data"]["code"] = (int)code;
    return ;
  }
  view_name << "errors/" << file_name.str() << ".ecpp";
  {
    View view(view_name.str());

    if (view.is_valid())
    {
      SharedVars  vars;
      View        layout("layouts/application.html.ecpp");
      std::string content;

      if (layout.is_valid())
      {
        std::string content_view = view.generate(vars);

        *vars["yield"] = &content_view;
        content = layout.generate(vars);
      }
      else
        content = view.generate(vars);
      out.set_headers("Content-Type", "text/html");
      Server::SetResponse(params, out, code, content);
      return ;
    }
    Server::SetResponse(params, out, code, "");
  }
}

void Server::run_request_parsers(const HttpServer::request& request, Response response, Params& params)
{
  RequestParsers::const_iterator handler_iterator = request_parsers.begin();

  for (; handler_iterator != request_parsers.end() ; ++ handler_iterator)
  {
    RequestParser::Status status;

    status = (**handler_iterator)(request, response, params);
    if (status == RequestParser::Stop)
      break ;
  }
}

void Server::run_request_handlers(const HttpServer::request& request, BuildingResponse& response, Params& params)
{
  RequestHandlers::const_iterator handler_iterator = request_handlers.begin();
  bool                            request_handled  = false;

  for (; handler_iterator != request_handlers.end() ; ++handler_iterator)
  {
    request_handled = (**handler_iterator)(request, response, params);
    if (request_handled)
      break ;
  }
  if (!request_handled)
    ResponseHttpError(response, HttpCodes::not_found, params);
}

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

void Server::operator()(const HttpServer::request& request, Response response)
{
  Utils::Timer     timer;
  Databases::singleton::Initialize();
  BuildingResponse out(response);
  Params           params;

  exception_catcher.run(out, params, [this, &request, &response, &out, &params]()
  {
    run_request_parsers(request, response, params);
    run_request_handlers(request, out, params);
  });
  Databases::singleton::Finalize();
  params["response-time"]["crails"] = timer.GetElapsedSeconds();
  post_request_log(params);
}

void Server::post_request_log(Params& params) const
{
  float crails_time     = params["response-time"]["crails"];
  float controller_time = params["response-time"]["controller"];
  unsigned short code   = params["response-data"]["code"];

  Logger::instance << Logger::Info << "# Responded to " << params["method"].Value() << " '" << params["uri"].Value();
  Logger::instance << "' with " << code;
  Logger::instance << " in " << crails_time << 's';
  if (params["response-time"]["controller"].NotNil())
    Logger::instance << " (controller: " << controller_time << "s)";
  Logger::instance << Logger::endl << Logger::endl;
}

void Server::log(const char* to_log)
{
  Logger::instance << Logger::Error << "[cpp-netlib] " << to_log << Logger::endl;
}

/*
 * Server Initialization
 */
#include <Boots/Utils/cl_options.hpp>
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
  Logger::instance << Logger::Info << "## Launching the amazing Crails Server ##" << Logger::endl;
  Utils::ClOptions options(argc, argv);

#ifdef USE_SEGVCATCH
  segvcatch::init_segv(&CrailsServer::ThrowCrashSegv);
#endif

  Router::singleton::Initialize();
  {
    Router* router = Router::singleton::Get();

    Logger::instance << Logger::Info << ">> Initializing routes" << Logger::endl;
    router->initialize();
    Logger::instance << Logger::Info << ">> Route initialized" << Logger::endl;
  }
  {
    std::string    address = options.GetValue("-h", "--hostname", std::string("127.0.0.1"));
    std::string    port    = options.GetValue("-p", "--port",     std::string("3001"));
#ifdef ASYNC_SERVER
    unsigned short threads = options.GetValue("-t", "--threads",  std::thread::hardware_concurrency());
#endif
    Logger::instance << ">> Initializing server" << Logger::endl;
    Logger::instance << ">> Listening on " << address << ":" << port << Logger::endl;

    Server              handler;
    HttpServer::options server_options(handler); 

    server_options.thread_pool();
#ifdef ASYNC_SERVER
    Logger::instance << ">> Pool Thread Size: " << threads << Logger::endl;
    thread_pool     _thread_pool(threads);
    thread_pool_ptr thread_pool_ptr(&_thread_pool);
    HttpServer      server(server_options.address(address.c_str()).port(port.c_str()).thread_pool(thread_pool_ptr));
#else
    HttpServer      server(server_options.address(address.c_str()).port(port.c_str()));
#endif

    signal(SIGINT, &shutdown);
    shutdown_lambda = [&server]()
    {
      Logger::instance << Logger::Info << ">> Crails server will shut down." << Logger::endl;
      Logger::instance << ">> Waiting for requests to end..." << Logger::endl;
      server.stop();
      Logger::instance << ">> Done." << Logger::endl;
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
  initialize_exception_catcher();
  initialize_request_pipe();
}
