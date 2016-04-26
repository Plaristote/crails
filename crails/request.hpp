#ifndef  CRAILS_REQUEST_HPP
# define CRAILS_REQUEST_HPP

# include "server.hpp"
# include "params.hpp"

namespace Crails
{
  struct Request
  {
    Request(const Server* server, const HttpServer::request&, Server::Response response);
    ~Request();

    const Server&              server;
    const HttpServer::request  request;
    int                        request_id;
    Params                     params;
    BuildingResponse           out;
    Utils::Timer               timer;
    ExceptionCatcher::Context  exception_context;

    void operator()();

    void run_parser(Server::RequestParsers::const_iterator, std::function<void(bool)>);
    void run_handler(Server::RequestHandlers::const_iterator, std::function<void(bool)>);

    void on_parsed(bool parsed);
    void on_handled(bool handled);
    void on_finished();
  };
}

#endif
