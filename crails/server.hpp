#ifndef  SERVER_HPP
# define SERVER_HPP

# include <boost/network/protocol/http/server.hpp>
# include "file_cache.hpp"
# include "multipart.hpp"
# include "backtrace.hpp"
# include <Boots/Utils/dynstruct.hpp>
# include "http_server.hpp"
# include "request_handler.hpp"

class Params;

struct CrailsServer : public CrailsServerTraits
{
  ~CrailsServer();
  
  typedef std::vector<RequestHandler*> RequestHandlers;
  
  struct Crash : public boost_ext::exception
  {
    Crash(const std::string& details) : details(details) {}
    
    const char* what(void) const throw()
    { return (details.c_str()); }
    
    std::string details;
  };
  
  void operator()(const Server::request& request, Response response);
  void log(const char* to_log);
  void post_request_log(Params& params) const;

  void add_request_handler(RequestHandler* request_handler)
  {
    request_handlers.push_back(request_handler);
  }
  
  RequestHandler* get_request_handler(const std::string& name) const;

  static void Launch(int argc, char** argv);
private:
  static void ThrowCrashSegv(void);
  static void ThrowCrashFpe(void);

  void initialize_request_pipe();

  void ReadRequestData(const Server::request& request, Response response, Params& params);
  void ParseResponse  (const Server::request& request, Response, Params& params);
  void ParseMultipart (const Server::request& request, Response, Params& params);
  void ParseForm      (const Server::request& request, Response, Params& params);

  void ResponseException(BuildingResponse& out, std::string exception_name, std::string exception_what, Params& params);
  void ResponseHttpError(BuildingResponse& out, CrailsServer::HttpCode code, Params& params);
  static void SetResponse(Params& params, BuildingResponse& out, CrailsServer::HttpCode code, const std::string& content);

#ifdef ASYNC_SERVER  
  typedef std::function<void (boost::iterator_range<char const*>,
                            boost::system::error_code,
                            size_t,
                            Server::connection_ptr)> ReadCallback;  
  
  ReadCallback    callback;
#endif

  RequestHandlers request_handlers;
  MultipartParser multipart_parser;
};

// Helpers
void        cgi2params(DynStruct& params, const std::string& encoded_str);
std::string GetMimeType(const std::string& filename);

#endif
