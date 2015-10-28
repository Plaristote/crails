#include "crails/exception_catcher.hpp"
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

void ExceptionCatcher::run(BuildingResponse& out, Params& params, std::function<void()> callback)
{
  Context context(out, params);
  
  context.iterator = 0;
  context.callback = callback;
  try
  {
    if (context.iterator < functions.size())
      functions[context.iterator](context);
    else
      callback();
  }
  catch (...)
  {
    response_exception(out, "Unknown exception", "Unfortunately, no data about it was harvested", params);
  }
}

void ExceptionCatcher::response_exception(BuildingResponse& out, string e_name, string e_what, Params& params)
{
  logger << Logger::Error << "# Catched exception " << e_name << ": " << e_what;
  if (params["backtrace"].exists())
    logger << "\n" << params["backtrace"].as<string>();
  logger << Logger::endl;
#ifdef SERVER_DEBUG
  SharedVars vars;

  vars["exception_name"] = e_name;
  vars["exception_what"] = e_what;
  vars["params"]         = &params;
  {
    Data response = params["response-data"];

    try {
      Renderer::render("lib/exception", params.as_data(), response, vars);
    }
    catch (MissingTemplate exception) {
      logger << Logger::Warning
        << "# Template lib/exception not found for format "
        << params["headers"]["Accept"].as<string>()
        << Logger::endl;
    }
    catch (...) {
      logger << Logger::Error << "# Template lib/exception crashed" << Logger::endl;
    }
    if (response["headers"]["Content-Type"].exists())
      out.set_headers("Content-Type", response["headers"]["Content-Type"].as<string>());
    Server::SetResponse(params, out, Server::HttpCodes::internal_server_error, response["body"].as<string>());
  }
#else
  Server::ResponseHttpError(out, Server::HttpCodes::internal_server_error, params);
#endif
}

void ExceptionCatcher::default_exception_handler(BuildingResponse& out, Params& params, const string exception_name, const string message, const string& trace)
{
  params["backtrace"] = trace;
  response_exception(out, exception_name, message, params);
}
