#include "crails/http_server/http_session.hpp"
#include "crails/http_server/websocket_session.hpp"
#include "crails/logger.hpp"
#include <boost/beast/websocket/rfc6455.hpp> // includes boost::beast::websocket::is_upgrade

using namespace Crails;

void HttpSession::run()
{
  wait_read();
}

void HttpSession::wait_read()
{
  parser.emplace();
  parser->body_limit(body_limit);
  stream.expires_after(std::chrono::seconds(timeout_in_seconds));
  boost::beast::http::async_read(
    stream,
    buffer,
    *parser,
    boost::beast::bind_front_handler(
      &HttpSession::on_read,
      shared_from_this()
    )
  );
}

void HttpSession::on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
{
  boost::ignore_unused(bytes_transferred);
  if (ec == boost::beast::http::error::end_of_stream)
    close_connection();
  else if (ec)
    Crails::logger << Crails::Logger::Error << "Crails::HttpSession::on_read: " << ec.message() << Crails::Logger::endl;
  else if (boost::beast::websocket::is_upgrade(parser->get()))
  {
    std::make_shared<Crails::WebsocketSession>(
      stream.release_socket()
    )->wait_accept(parser->release());
  }
  else
  {
    // Handle request
    if (!queue.is_full())
      wait_read();
  }
}

void HttpSession::on_write(bool close, boost::beast::error_code ec, std::size_t bytes_transferred)
{
  boost::ignore_unused(bytes_transferred);

  if (ec)
    logger << Logger::Error << "Crails::HttpSession::on_write: " << ec.message() << Logger::endl;
  else if (close)
    close_connection();
  else if (queue.on_write())
    wait_read();
}

void HttpSession::close_connection()
{
  boost::beast::error_code ec;

  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
}
