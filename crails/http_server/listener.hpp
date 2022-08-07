#ifndef  CRAILS_HTTP_LISTENER_H
# define CRAILS_HTTP_LISTENER_H

# include <boost/asio/ip/tcp.hpp>
# include <boost/beast/core.hpp>

namespace Crails
{
  class Connection;

  class Listener : public std::enable_shared_from_this<Listener>
  {
    boost::asio::ip::tcp::acceptor    acceptor;
    boost::asio::ip::tcp::socket      socket;
    std::function<void (Connection&)> handler;
  public:
    Listener(boost::asio::io_context&);

    bool listen(boost::asio::ip::tcp::endpoint, boost::beast::error_code&);
    void run();
    void set_handler(std::function<void(Connection&)>
    value) { handler = value; }
  private:
    void wait_accept();
    void on_accept(boost::beast::error_code);
  };
}

#endif // LISTENER_H
