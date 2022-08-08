#include <crails/http_server/listener.hpp>
#include <crails/server.hpp>
#include <crails/router.hpp>
#include <crails/params.hpp>
#include <crails/logger.hpp>
#include <crails/program_options.hpp>
#include <crails/request_parser.hpp>
#include <crails/request_handler.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio/signal_set.hpp>
#ifdef USE_SEGVCATCH
# include <segvcatch.h>
# define initialize_segvcatch(listener) segvcatch::init_segv(listener)
#else
# define initialize_segvcatch(listener)
#endif

using namespace std;
using namespace Crails;

Crails::FileCache       Server::file_cache;
Server::RequestParsers  Server::request_parsers;
Server::RequestHandlers Server::request_handlers;
shared_ptr<boost::asio::io_context> Server::io_context;

const string Server::public_path = boost::filesystem::canonical(
  boost::filesystem::current_path().string() + "/public"
).string();

Server::Server(unsigned short thread_count)
{
  logger << ">> Pool Thread Size: " << thread_count << Logger::endl;
  io_context = make_shared<boost::asio::io_context>(thread_count);
  initialize_exception_catcher();
  initialize_request_pipe();
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

void Server::Launch(int argc, const char **argv)
{
  logger << Logger::Info << "## Launching the amazing Crails Server ##" << Logger::endl;
  const ProgramOptions          options(argc, argv);
  SingletonInstantiator<Router> router;
  {
    Server server(options.get_thread_count());
    auto   listener = make_shared<Listener>(server);
    boost::asio::signal_set  signals(get_io_context(), SIGINT, SIGTERM);
    boost::beast::error_code error;

    logger << Logger::Info << ">> Initializing routes" << Logger::endl;
    router->initialize();
    logger << Logger::Info << ">> Route initialized" << Logger::endl;
    signals.async_wait(bind(&Server::Stop));
    if (listener->listen(options.get_endpoint(), error))
    {
      server.initialize_pid_file(options.get_pidfile_path());
      listener->run();
      initialize_segvcatch(&CrailsServer::ThrowCrashSegv);
      get_io_context().run();
    }
    else
      logger << Logger::Error << "!! Could not listen on endpoint: " << error.message() << Logger::endl;
  }
}

void Server::Stop()
{
  logger << Logger::Info << ">> Crails server will shut down." << Logger::endl;
  logger << ">> Waiting for requests to end..." << Logger::endl;
  get_io_context().stop();
  logger << ">> Done." << Logger::endl;
}

void Server::initialize_pid_file(const string& filepath) const
{
  ofstream output(filepath.c_str());

  if (output.is_open())
  {
    pid_t pid = getpid();

    logger << ">> PID " << pid << " (stored in " << filepath << ')' << Logger::endl;
    output << pid;
    output.close();
  }
  else
    logger << "!! Failed to open PID file " << filepath << Logger::endl;
}

void Server::initialize_exception_catcher()
{
  exception_catcher.add_exception_catcher<std::exception&>("std::exception");
}

void Server::add_request_handler(RequestHandler* request_handler)
{
  request_handlers.push_back(request_handler);
}

void Server::add_request_parser(RequestParser* request_parser)
{
  request_parsers.push_back(request_parser);
}

RequestHandler* Server::get_request_handler(const string& name)
{
  auto compare = [name](const RequestHandler* handler) { return handler->get_name() == name; };
  auto it = std::find_if(request_handlers.cbegin(), request_handlers.cend(), compare);

  return it != request_handlers.cend() ? *it : nullptr;
}

void Server::ThrowCrashSegv()
{
  throw Server::Crash("Segmentation Fault");
}
