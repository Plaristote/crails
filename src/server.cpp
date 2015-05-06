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

#ifdef ASYNC_SERVER
# pragma message("Building Asynchronous Server")
#else
# pragma message("Building Synchronous Server")
#endif

#ifdef SERVER_DEBUG
# pragma message("Building Testing Server")
#else
# pragma message("Building Release Server")
#endif

using namespace std;

void CrailsServer::SetResponse(Params& params, BuildingResponse& out, CrailsServer::HttpCode code, const string& content)
{
  out.SetResponse(code, content);
  params["response-data"]["code"]   = (int)code;
  params["response-data"]["length"] = content.size();
}

void CrailsServer::ResponseException(BuildingResponse& out, std::string e_name, std::string e_what, Params& params)
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
    std::string content = view.Generate(vars);

    out.SetHeaders("Content-Type", "text/html");
    SetResponse(params, out, CrailsServer::HttpCodes::internal_server_error, content);
  }
#else
  ResponseHttpError(out, CrailsServer::HttpCodes::internal_server_error, params);
#endif
}

RequestHandler* CrailsServer::get_request_handler(const string& name) const
{
  auto it = request_handlers.begin();
  
  for (; it != request_handlers.end() ; ++it)
  {
    RequestHandler* request_handler = *it;

    if (request_handler->get_name() == name)
      return request_handler;
  }
  return 0;
}

void CrailsServer::ResponseHttpError(BuildingResponse& out, CrailsServer::HttpCode code, Params& params)
{
  FileRequestHandler* file_handler = reinterpret_cast<FileRequestHandler*>(get_request_handler("file"));
  std::stringstream file_name;
  std::stringstream view_name;

  file_name << (unsigned int)(code) << ".html";
  if (file_handler && file_handler->SendFile("../public/" + file_name.str(), out, code, 0))
  {
    params["response-data"]["code"] = (int)code;
    return ;
  }
  view_name << "errors/" << file_name.str() << ".ecpp";
  {
    View view(view_name.str());
      
    if (view.IsValid())
    {
      SharedVars  vars;
      View        layout("layouts/application.html.ecpp");
      std::string content;

      if (layout.IsValid())
      {
        std::string content_view = view.Generate(vars);
         
        *vars["yield"] = &content_view;
        content = layout.Generate(vars);
      }
      else
        content = view.Generate(vars);
      out.SetHeaders("Content-Type", "text/html");
      SetResponse(params, out, code, content);
      return ;
    }
    SetResponse(params, out, code, "");
  }
}

void CrailsServer::ReadRequestData(const Server::request& request, Response response, Params& params)
{
  const char*  get_params = strrchr(request.destination.c_str(), '?');
  std::string  uri        = request.destination;

  // Setting Headers parameters
  {
    auto it  = request.headers.begin();
    auto end = request.headers.end();

    for (; it != end ; ++it)
    {
      boost::network::http::request_header_narrow header = *it;
      params["header"][header.name] = header.value;
    }      
  }

  // Getting get parameters
  if (get_params != 0)
  {
    std::string str_params(get_params);

    uri.erase(uri.size() - str_params.size());
    str_params.erase(0, 1);
    cgi2params(params, str_params);
  }
  
  // Reading body if relevant
  if (request.method != "GET")
    ParseResponse(request, response, params);
  
  // Set URI and method for the posterity (is that even a word ?)
  params["uri"]    = uri;
  params["method"] = request.method;
}

void CrailsServer::operator()(const Server::request& request, Response response)
{
  Utils::Timer     timer;
  Databases::singleton::Initialize();
  BuildingResponse out(response);
  Params           params;

  try
  {
    RequestHandlers::const_iterator handler_iterator = request_handlers.begin();
    
    ReadRequestData(request, response, params);
    for (; handler_iterator != request_handlers.end() ; ++handler_iterator)
    {
      if ((**handler_iterator)(request, out, params))
        break ;
    }
  }
  catch (const Router::Exception302 e)
  {
    out.SetHeaders("Location", e.redirect_to);
    params.session->Finalize(out);
    ResponseHttpError(out, HttpCodes::moved_temporarily, params);
  }
  catch (const Router::Exception404 e)
  {
    ResponseHttpError(out, HttpCodes::not_found, params);
  }
#define EXCEPTION_RESPONSE(type,set_params) \
  catch (const type e) \
  { \
    set_params; \
    params["backtrace"] = boost_ext::trace(e); \
    ResponseException(out, #type, e.what(), params); \
  }
  EXCEPTION_RESPONSE(CrailsServer::Crash,)
  EXCEPTION_RESPONSE(std::exception&,)
  catch (...)
  {
    ResponseException(out, "Unknown exception", "Unfortunately, no data about it was harvested", params);
  }
  Databases::singleton::Finalize();
  params["response-time"]["crails"] = timer.GetElapsedSeconds();
  post_request_log(params);
}

void CrailsServer::post_request_log(Params& params) const
{
  float crails_time     = params["response-time"]["crails"];
  float controller_time = params["response-time"]["controller"];
  unsigned short code   = params["response-data"]["code"];
  

  cout << "# Responded to " << params["method"].Value() << " '" << params["uri"].Value();
  cout << "' with " << code;
  cout << " in " << crails_time << 's';
  if (params["response-time"]["controller"].NotNil())
    cout << " (controller: " << controller_time << "s)";
  cout << endl << endl;
}

void CrailsServer::log(const char* to_log)
{
  cout << "[Boost][Net] " << to_log << endl;
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
void CrailsServer::ThrowCrashSegv()
{
  throw CrailsServer::Crash("Segmentation Fault");
}
#endif

std::function<void (void)> shutdown_lambda;
static void shutdown(int) { shutdown_lambda(); }

void CrailsServer::Launch(int argc, char **argv)
{
  cout << "## Launching the amazing Crails Server ##" << endl;
  Utils::ClOptions options(argc, argv);

#ifdef USE_SEGVCATCH
  segvcatch::init_segv(&CrailsServer::ThrowCrashSegv);
#endif

  Router::singleton::Initialize();
  {
    Router* router = Router::singleton::Get();

    cout << ">> Initializing routes" << endl;
    router->Initialize();
    cout << ">> Route initialized" << endl;
  }
  {
    std::string    address = options.GetValue("-h", "--hostname", std::string("127.0.0.1"));
    std::string    port    = options.GetValue("-p", "--port",     std::string("3001"));
#ifdef ASYNC_SERVER
    unsigned short threads = options.GetValue("-t", "--threads",  std::thread::hardware_concurrency());
#endif
    cout << ">> Initializing server" << endl;
    cout << ">> Listening on " << address << ":" << port << endl;

    CrailsServer    handler;
    Server::options server_options(handler); 

    handler.initialize_request_pipe();
    server_options.thread_pool();
#ifdef ASYNC_SERVER
    cout << ">> Pool Thread Size: " << threads << endl;
    thread_pool     _thread_pool(threads);
    thread_pool_ptr thread_pool_ptr(&_thread_pool);
    Server          server(server_options.address(address.c_str()).port(port.c_str()).thread_pool(thread_pool_ptr));
#else
    Server          server(server_options.address(address.c_str()).port(port.c_str()));
#endif

    signal(SIGINT, &shutdown);
    shutdown_lambda = [&server]()
    {
      cout << ">> Crails server will shut down." << endl;
      cout << ">> Waiting for requests to end..." << endl;
      server.stop();
      cout << ">> Done." << endl;
    };
    server.run();
  }
  Router::singleton::Finalize();  
}

CrailsServer::~CrailsServer()
{
  for_each(request_handlers.begin(), request_handlers.end(), [](RequestHandler* request_handler)
  {
    delete request_handler;
  });
}