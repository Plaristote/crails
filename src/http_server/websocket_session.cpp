#include "crails/http_server/websocket_session.hpp"
#include "crails/logger.hpp"

using namespace Crails;

void WebsocketSession::on_accept(boost::beast::error_code ec)
{
  if (ec)
    logger << Logger::Error << "Crails::WebsocketSession::on_accept: " << ec.message() << Logger::endl;
  else
    wait_read();
}

void WebsocketSession::wait_read()
{
  websocket.async_read(
    buffer,
    boost::beast::bind_front_handler(
      &WebsocketSession::on_read,
      shared_from_this()
    )
  );
}

void WebsocketSession::on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
{
      boost::ignore_unused(bytes_transferred);
      if (ec == boost::beast::websocket::error::closed)
        return ;
      else if (ec)
        Crails::logger << Crails::Logger::Error << "Crails::WebsocketSession::on_read: " << ec.message() << Crails::Logger::endl;
      else
      {
        // Echo the message (perhaps we should do something else over here)
        websocket.text(websocket.got_text());
        websocket.async_write(
          buffer.data(),
          boost::beast::bind_front_handler(
            &WebsocketSession::on_write,
            shared_from_this()
          )
        );
      }
}

void WebsocketSession::on_write(boost::beast::error_code ec, std::size_t bytes_transferred)
{
  boost::ignore_unused(bytes_transferred);
  if (ec)
    logger << Logger::Error << "Crails::WebsocketSession::on_write: " << ec.message() << Logger::endl;
  buffer.consume(buffer.size());
  wait_read();
}
