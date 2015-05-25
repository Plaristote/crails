#include "crails/exception_catcher.hpp"
#include "crails/server.hpp"
#include "crails/params.hpp"
#ifdef SERVER_DEBUG
# include <crails/view.hpp>
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
  cerr << "# Catched exception " << e_name << ": " << e_what << endl;
  if (params["backtrace"].NotNil())
    cerr << params["backtrace"].Value() << endl;
#ifdef SERVER_DEBUG
  View       view("../../lib/exception.html.ecpp");
  SharedVars vars;

  *vars["exception_name"] = &e_name;
  *vars["exception_what"] = &e_what;
  *vars["params"]         = &params;
  {
    std::string content = view.generate(vars);

    out.set_headers("Content-Type", "text/html");
    Server::SetResponse(params, out, Server::HttpCodes::internal_server_error, content);
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
