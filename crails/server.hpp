#ifndef  SERVER_HPP
# define SERVER_HPP

# include <boost/network/protocol/http/server.hpp>
# include "file_cache.hpp"
# include "multipart.hpp"
# include "backtrace.hpp"
# include <Boots/Utils/dynstruct.hpp>

namespace http = boost::network::http;

class  BuildingResponse;
struct CrailsServer;

# ifdef ASYNC_SERVER
typedef http::async_server<CrailsServer> Server;

struct CrailsServerTraits
{
  typedef Server::connection::status_t HttpCode;
  typedef Server::connection           HttpCodes;
  typedef Server::connection_ptr       Response;
};
#else
typedef http::server<CrailsServer>       Server;

struct CrailsServerTraits
{
  typedef Server::response::status_type HttpCode;
  typedef Server::response              HttpCodes;
  typedef Server::response&             Response;
};
#endif

class Params;

struct CrailsServer : public CrailsServerTraits
{
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

  static void Launch(int argc, char** argv);
private:
  static void ThrowCrashSegv(void);
  static void ThrowCrashFpe(void);

  void ReadRequestData(const Server::request& request, Response response, Params& params);
  void ParseResponse  (const Server::request& request, Response, Params& params);
  void ParseMultipart (const Server::request& request, Response, Params& params);
  void ParseForm      (const Server::request& request, Response, Params& params);

  bool ServeFile  (const Server::request& request, BuildingResponse& response, Params& params);
  bool ServeAction(const Server::request& request, BuildingResponse& response, Params& params);

  bool SendFile(const std::string& path, BuildingResponse& response, CrailsServer::HttpCode code, unsigned int first_bit = 0);

  void ResponseException(BuildingResponse& out, std::string exception_name, std::string exception_what, Params& params);
  void ResponseHttpError(BuildingResponse& out, CrailsServer::HttpCode code, Params& params);
  void SetResponse(Params& params, BuildingResponse& out, CrailsServer::HttpCode code, const std::string& content);

  FileCache file_cache;

#ifdef ASYNC_SERVER  
  typedef std::function<void (boost::iterator_range<char const*>,
                            boost::system::error_code,
                            size_t,
                            Server::connection_ptr)> ReadCallback;  
  
  ReadCallback    callback;
#endif
  MultipartParser multipart_parser;
};

// Helpers
void        cgi2params(DynStruct& params, const std::string& encoded_str);
std::string GetMimeType(const std::string& filename);

#endif
