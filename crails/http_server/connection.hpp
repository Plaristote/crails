#ifndef  CRAILS_HTTP_CONNECTION_H
# define CRAILS_HTTP_CONNECTION_H

# include "crails/http_server/types.hpp"
# include <boost/asio/ip/tcp.hpp>
# include <boost/beast/core.hpp>

namespace Crails
{
  class Connection : public std::enable_shared_from_this<Connection>
  {
  public:
    typedef std::function<void(Connection&)> RequestHandler;

    Connection(boost::asio::ip::tcp::socket, RequestHandler);
    ~Connection();

    void start();
    void write();
    void close();

    const HttpRequest& get_request() const { return request; }
    HttpResponse&      get_response() { return response; }

  private:
    void expect_read();
    void read(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_write(bool keep_alive, boost::beast::error_code ec, std::size_t);

    boost::beast::tcp_stream  stream;
    boost::beast::flat_buffer buffer{8192};
    HttpRequest               request{};
    HttpResponse              response{};
    RequestHandler            handler;
  };
}

#endif // CONNECTION_H
