#ifndef  CRAILS_LISTENER_HPP
# define CRAILS_LISTENER_HPP

# include <memory>
# include <boost/beast/core.hpp>

namespace Crails
{
  class Listener : public std::enable_shared_from_this<Listener>
  {
    boost::asio::io_context&       io_context;
    boost::asio::ip::tcp::acceptor acceptor;
  public:
    Listener(boost::asio::io_context& io_context, boost::asio::ip::tcp::endpoint endpoint);

    void run();
  private:
    void wait_accept();
    void on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
    void on_initialization_failure(boost::beast::error_code);
  };
}

#endif
