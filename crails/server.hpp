#ifndef  SERVER_HPP
# define SERVER_HPP

# include "http.hpp"
# include "file_cache.hpp"
# include "exception_catcher.hpp"
# include "datatree.hpp"

namespace Crails
{
  class Params;
  class Context;
  class RequestParser;
  class RequestHandler;
  class Connection;

  class Server
  {
    friend class Context;
  public:
    static const std::string temporary_path;
    static const std::string public_path;

    Server(unsigned short thread_count = 1);
    ~Server();

    typedef HttpStatus                   HttpCodes; // Deprecated
    typedef std::vector<RequestParser*>  RequestParsers;
    typedef std::vector<RequestHandler*> RequestHandlers;

    struct Crash : public boost_ext::exception
    {
      Crash(const std::string& details) : details(details) {}
      const char* what(void) const throw() { return (details.c_str()); }
      std::string details;
    };

    static void Launch(int argc, const char** argv);
    static void Stop();

    static RequestHandler*          get_request_handler(const std::string& name);
    static FileCache&               get_file_cache() { return file_cache; }
    static boost::asio::io_context& get_io_context() { return *(io_context.get()); }

  private:
    static void ThrowCrashSegv();

    void initialize_exception_catcher();
    void initialize_pid_file(const std::string&) const;
    void initialize_request_pipe();
    void add_request_handler(RequestHandler* request_handler);
    void add_request_parser(RequestParser* request_parser);
    void on_interrupted(const boost::system::error_code&, int);

    static std::shared_ptr<boost::asio::io_context> io_context;
    static RequestParsers    request_parsers;
    static RequestHandlers   request_handlers;
    static FileCache         file_cache;
    ExceptionCatcher         exception_catcher;
  };
}

#endif
