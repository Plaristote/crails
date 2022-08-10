#ifndef  CRAILS_REQUEST_HPP
# define CRAILS_REQUEST_HPP

# include "server.hpp"
# include "params.hpp"
# include "http_server/connection.hpp"
# include "http_response.hpp"
# include "exception_catcher.hpp"
# include "utils/timer.hpp"

namespace Crails
{
  class ExceptionCatcher;
  namespace Tests { class Request; }

  class Context : public std::enable_shared_from_this<Context>
  {
    friend class ExceptionCatcher;
    friend class Connection;
    friend class Tests::Request;
    const Server&             server;
    bool                      handled = false;
    ExceptionCatcher::Context exception_context;
  public:
    Context(const Server& server, Connection& connection);
    ~Context();

    std::shared_ptr<Connection> connection;
    BuildingResponse            response;
    Params                      params;
    Utils::Timer                timer;

  private:
    void run();
    void run_parser(Server::RequestParsers::const_iterator, std::function<void(bool)>);
    void run_handler(Server::RequestHandlers::const_iterator, std::function<void(bool)>);

    void on_parsed(bool parsed);
    void on_handled(bool handled);
    void on_finished();
  };
}

#endif
