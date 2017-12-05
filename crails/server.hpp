#ifndef  SERVER_HPP
# define SERVER_HPP

# include "file_cache.hpp"
# include "http_server.hpp"
# include "request_parser.hpp"
# include "request_handler.hpp"
# include "exception_catcher.hpp"
# include "datatree.hpp"
# include "utils/timer.hpp"

namespace Crails
{
  class Params;
  class Request;

  class Server : public ServerTraits
  {
  public:
    static const std::string temporary_path;
    static const std::string public_path;

    friend class ExceptionCatcher;
    friend class Request;

    Server();
    ~Server();

    typedef std::vector<RequestParser*>  RequestParsers;
    typedef std::vector<RequestHandler*> RequestHandlers;

    struct Crash : public boost_ext::exception
    {
      Crash(const std::string& details) : details(details) {}

      const char* what(void) const throw()
      { return (details.c_str()); }

      std::string details;
    };

    void operator()(const HttpServer::request& request, Response response);
    void log(const char* to_log);

    void                   add_request_handler(RequestHandler* request_handler);
    void                   add_request_parser(RequestParser* request_parser);
    static RequestHandler* get_request_handler(const std::string& name);
    static FileCache&      get_file_cache() { return file_cache; }
    static std::shared_ptr<boost::asio::io_service> get_io_service() { return io_service; }

    static void Launch(int argc, char** argv);

    static void SetResponse(Params& params, BuildingResponse& out, Server::HttpCode code, const std::string& content);
  private:
    static void ThrowCrashSegv(void);
    static void ThrowCrashFpe(void);

    void initialize_request_pipe();
    void initialize_exception_catcher();
    void run_request_parsers (RequestParsers::const_iterator, const HttpServer::request&, BuildingResponse&, Params&, std::function<void(bool)>) const;
    void run_request_handlers(RequestHandlers::const_iterator, const HttpServer::request&, BuildingResponse&, Params&, std::function<void(bool)>) const;

    static void ResponseHttpError(BuildingResponse& out, Server::HttpCode code, Params& params);

    static std::shared_ptr<boost::asio::io_service> io_service;
    static RequestParsers    request_parsers;
    static RequestHandlers   request_handlers;
    static FileCache         file_cache;
    ExceptionCatcher         exception_catcher;
  };
}

// Helpers
void        cgi2params(Data params, const std::string& encoded_str);
std::string get_mimetype(const std::string& filename);

#endif
