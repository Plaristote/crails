#ifndef  CRAILS_WEBSOCKET_SESSION_HPP
# define CRAILS_WEBSOCKET_SESSION_HPP

# include <memory>
# include <boost/beast/core.hpp>
# include <boost/beast/http.hpp>
# include <boost/beast/websocket.hpp>
# include <boost/beast/version.hpp>

namespace Crails
{
  class WebsocketSession : public std::enable_shared_from_this<WebsocketSession>
  {
    boost::beast::websocket::stream<boost::beast::tcp_stream> websocket;
    boost::beast::flat_buffer buffer;

  public:
    explicit WebsocketSession(boost::asio::ip::tcp::socket&& socket) : websocket(std::move(socket))
    {
    }

    template<class Body, class Allocator>
    void wait_accept(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator> > request)
    {
      websocket.set_option(
        boost::beast::websocket::stream_base::timeout::suggested(
          boost::beast::role_type::server
        )
      );
      websocket.set_option(
        boost::beast::websocket::stream_base::decorator(
          [](boost::beast::websocket::response_type& response)
          {
            response.set(boost::beast::http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " crails-server");
          }
        )
      );
      websocket.async_accept(
        request,
        boost::beast::bind_front_handler(
          &WebsocketSession::on_accept,
          shared_from_this()
        )
      );
    }

  private:
    void on_accept(boost::beast::error_code ec);
    void wait_read();
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);
  };
}

#endif
