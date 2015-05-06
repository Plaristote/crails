#ifndef  SERVER_HPP
# define SERVER_HPP

# include "file_cache.hpp"
# include "multipart.hpp"
# include "backtrace.hpp"
# include <Boots/Utils/dynstruct.hpp>
# include "http_server.hpp"
# include "request_parser.hpp"
# include "request_handler.hpp"
# include "exception_catcher.hpp"

class Params;

struct CrailsServer : public CrailsServerTraits
{
  friend class ExceptionCatcher;
  
  CrailsServer();
  ~CrailsServer();
  
  typedef std::vector<RequestParser*>  RequestParsers;
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

  void                   add_request_handler(RequestHandler* request_handler);
  void                   add_request_parser(RequestParser* request_parser);
  static RequestHandler* get_request_handler(const std::string& name);

  static void Launch(int argc, char** argv);

  static void SetResponse(Params& params, BuildingResponse& out, CrailsServer::HttpCode code, const std::string& content);
private:
  static void ThrowCrashSegv(void);
  static void ThrowCrashFpe(void);

  void initialize_request_pipe();
  void initialize_exception_catcher();
  void run_request_parsers (const Server::request&, Response, Params&);
  void run_request_handlers(const Server::request&, BuildingResponse&, Params&);

  static void ResponseHttpError(BuildingResponse& out, CrailsServer::HttpCode code, Params& params);

  static RequestParsers    request_parsers;
  static RequestHandlers   request_handlers;
  MultipartParser  multipart_parser;
  Crails::ExceptionCatcher exception_catcher;
};

// Helpers
void        cgi2params(DynStruct& params, const std::string& encoded_str);
std::string GetMimeType(const std::string& filename);

#endif
