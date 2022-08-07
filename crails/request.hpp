#ifndef  CRAILS_REQUEST_HPP
# define CRAILS_REQUEST_HPP

# include "server.hpp"
# include "params.hpp"
# include "http_server/connection.hpp"

namespace Crails
{
  class Request : public std::enable_shared_from_this<Request>
  {
  public:
    Request(const Server* server, Connection& connection);
    ~Request();

    const Server&             server;
    Connection&               connection;
    BuildingResponse          out;
    int                       request_id;
    Params                    params;
    Utils::Timer              timer;
    ExceptionCatcher::Context exception_context;

    void operator()();

    void run_parser(Server::RequestParsers::const_iterator, std::function<void(bool)>);
    void run_handler(Server::RequestHandlers::const_iterator, std::function<void(bool)>);

    void on_parsed(bool parsed);
    void on_handled(bool handled);
    void on_finished();
  };
}

#endif
