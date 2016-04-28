#include "crails/exception_catcher.hpp"
#include "crails/request.hpp"
#include "crails/server.hpp"
#include "crails/params.hpp"
#include "crails/logger.hpp"
#ifdef SERVER_DEBUG
# include "crails/shared_vars.hpp"
# include "crails/renderer.hpp"
#endif

using namespace std;
using namespace Crails;

ExceptionCatcher::ExceptionCatcher()
{}

void ExceptionCatcher::run_protected(Request& request, std::function<void()> callback) const
{
  Context context(request);
  
  context.iterator          = 0;
  context.callback          = callback;
  context.thread_id         = std::this_thread::get_id();
  request.exception_context = context;

  try
  {
    if (context.iterator < functions.size())
      functions[context.iterator](context);
    else
      callback();
  }
  catch (...)
  {
    response_exception(request, "Unknown exception", "Unfortunately, no data about it was harvested");
    request.on_finished();
    return ;
  }
}

void ExceptionCatcher::run(Request& request, std::function<void()> callback) const
{
  if (request.exception_context.thread_id != std::this_thread::get_id())
  {
    run_protected(request, callback);
    if (request.exception_context.thread_id == std::this_thread::get_id())
      request.exception_context.thread_id = std::thread::id();
  }
  else
    callback();
}

void ExceptionCatcher::response_exception(Request& request, string e_name, string e_what) const
{
  logger << Logger::Error << "# Catched exception " << e_name << ": " << e_what;
  if (request.params["backtrace"].exists())
    logger << "\n" << request.params["backtrace"].as<string>();
  logger << Logger::endl;
#ifdef SERVER_DEBUG
  SharedVars vars;

  vars["exception_name"] = e_name;
  vars["exception_what"] = e_what;
  vars["params"]         = &(request.params);
  {
    Data response = request.params["response-data"];

    try {
      Renderer::render("lib/exception", request.params.as_data(), response, vars);
    }
    catch (MissingTemplate exception) {
      logger << Logger::Warning
        << "# Template lib/exception not found for format "
        << request.params["headers"]["Accept"].defaults_to<string>("")
        << Logger::endl;
    }
    catch(std::exception e) {
      logger << Logger::Error << "# Template lib/exception crashed (" << e.what() << ')' << Logger::endl;
    }
    catch (...) {
      logger << Logger::Error << "# Template lib/exception crashed" << Logger::endl;
    }
    if (response["headers"]["Content-Type"].exists())
      request.out.set_headers("Content-Type", response["headers"]["Content-Type"].as<string>());
    Server::SetResponse(request.params, request.out, Server::HttpCodes::internal_server_error, response["body"].defaults_to<string>(""));
  }
#else
  Server::ResponseHttpError(request.out, Server::HttpCodes::internal_server_error, request.params);
#endif
  request.params["response-data"]["code"] = Server::HttpCodes::internal_server_error;
  request.on_finished();
}

void ExceptionCatcher::default_exception_handler(Request& request, const string exception_name, const string message, const string& trace)
{
  request.params["backtrace"] = trace;
  response_exception(request, exception_name, message);
}
