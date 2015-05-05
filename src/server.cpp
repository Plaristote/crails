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
  View       view("../../lib/exception.html.ecpp");
  SharedVars vars;
  
  *vars["exception_name"] = &e_name;
  *vars["exception_what"] = &e_what;
  *vars["params"]         = &params;
  {
    std::string content = view.Generate(vars);

    cerr << "# Catched exception " << e_name << ": " << e_what << endl;
    cerr << params["backtrace"].Value() << endl;
#ifdef SERVER_DEBUG
    out.SetHeaders("Content-Type", "text/html");
    SetResponse(params, out, CrailsServer::HttpCodes::internal_server_error, content);
#else
    ResponseHttpError(out, CrailsServer::HttpCodes::internal_server_error, params);
#endif
  }
}

void CrailsServer::ResponseHttpError(BuildingResponse& out, CrailsServer::HttpCode code, Params& params)
{
  std::stringstream file_name;
  std::stringstream view_name;

  file_name << (unsigned int)(code) << ".html";
  if (SendFile("../public/" + file_name.str(), out, code, 0))
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
    ReadRequestData(request, response, params);
    if (request.method == "GET" && ServeFile(request, out, params))
      params["response-data"]["code"] = (int)HttpCodes::ok;
    else
    {
      if (!(ServeAction(request, out, params)))
        ResponseException(out, "Router", "Crails Router isn't initialized", params);
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
 * Server Actions
 */
bool CrailsServer::SendFile(const std::string& fullpath, BuildingResponse& response, CrailsServer::HttpCode code, unsigned int first_bit)
{
  file_cache.Lock();
  {
#ifndef SERVER_DEBUG
    bool               cached = file_cache.Contains(fullpath);
#endif
    const std::string& str = *(file_cache.Require(fullpath));

    if (&str != 0)
    {
      std::stringstream str_length;

      str_length << (str.size() - first_bit);
      response.SetHeaders("Content-Length", str_length.str());
      response.SetHeaders("Content-Type",   GetMimeType(strrchr(fullpath.c_str(), '.')));
      response.SetStatusCode(code);
      response.SetBody(str.c_str() + first_bit, str.size() - first_bit);
      cout << "# Delivering asset `" << fullpath << "` ";
#ifdef SERVER_DEBUG
      file_cache.GarbageCollect();
      cout << "(cache disabled)" << endl;
#else
      cout << (cached ? "(was cached)" : "(was not cached)") << endl;
#endif
      file_cache.Unlock();
      return (true);
    }
  }
  file_cache.Unlock();
  return (false);
}

bool CrailsServer::ServeFile(const Server::request& request, BuildingResponse& response, Params& params)
{
  std::string fullpath       = params["uri"].Value();
  size_t      pos_get_params = fullpath.find('?');

  if (pos_get_params != std::string::npos)
    fullpath.erase(pos_get_params);
  fullpath = "../public" + fullpath;

  { // Is a directory ?
    Directory dir;
    
    if (dir.OpenDir(fullpath))
      fullpath += "/index.htm";
  }
  return (SendFile(fullpath, response, CrailsServer::HttpCodes::ok));
}

bool CrailsServer::ServeAction(const Server::request& request, BuildingResponse& out, Params& params)
{
  const Router* router = Router::singleton::Get();

  if (router)
  {
    string         method = (params["_method"].Nil() ? request.method : params["_method"].Value());
    Router::Action action = router->get_action(method, params["uri"].Value(), params);

    params.session->Load(params["header"]);
    {
      DynStruct     data   = action(params);
      string        body   = data["body"].Value();
      HttpCode      code   = HttpCodes::ok;

      out.SetHeaders("Content-Type", "text/html");
      if (data["headers"].NotNil())
      { // If parameters headers were set, copy them to the response
        auto it  = data["headers"].begin();
        auto end = data["headers"].end();

        for (; it != end ; ++it)
          out.SetHeaders((*it).Key(), (*it).Value());
      }
      params.session->Finalize(out);
      if (data["status"].NotNil())
        code = (HttpCode)((int)data["status"]);
      SetResponse(params, out, code, body);
    }
    return (true);
  }
  return (false);
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
