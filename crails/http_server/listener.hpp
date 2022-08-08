#ifndef  CRAILS_HTTP_LISTENER_H
# define CRAILS_HTTP_LISTENER_H

# include <boost/asio/ip/tcp.hpp>
# include <boost/beast/core.hpp>

namespace Crails
{
  class Connection;
  class Server;

  class Listener : public std::enable_shared_from_this<Listener>
  {
    const Server&                  server;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket   socket;
  public:
    Listener(const Server&);

    bool listen(boost::asio::ip::tcp::endpoint, boost::beast::error_code&);
    void run();
  private:
    void wait_accept();
    void on_accept(boost::beast::error_code);
  };
}

#endif // LISTENER_H
