#include <boost/asio/dispatch.hpp>
#include "crails/http_server/connection.hpp"
#include "crails/logger.hpp"

using namespace Crails;
using namespace boost;

Connection::Connection(asio::ip::tcp::socket socket_, RequestHandler handler_) :
  stream(std::move(socket_)),
  handler(handler_)
{
}

Connection::~Connection()
{
  logger << Logger::Info << "Crails::Session closed" << Logger::endl;
}

void Connection::start()
{
  asio::dispatch(
    stream.get_executor(),
    beast::bind_front_handler(&Connection::expect_read, shared_from_this())
  );
}

void Connection::expect_read()
{
  request = {};
  stream.expires_after(std::chrono::seconds(30));
  beast::http::async_read(
    stream, buffer, request,
    beast::bind_front_handler(&Connection::read, shared_from_this())
  );
}

void Connection::read(beast::error_code ec, std::size_t bytes_transferred)
{
  if (!ec)
  {
    response.keep_alive(request.keep_alive());
    handler(*this);
  }
  else
    logger << Logger::Error << "!! Crails failed to read on socket: " << ec.message() << Logger::endl;
}

void Connection::write()
{
  response.content_length(response.body().size());
  beast::http::async_write(
    stream, response,
    beast::bind_front_handler(&Connection::on_write, shared_from_this(), response.keep_alive())
  );
}

void Connection::on_write(bool keep_alive, beast::error_code ec, std::size_t)
{
  if (!ec && keep_alive)
    expect_read();
  else
    close();
}

void Connection::close()
{
  beast::error_code ec;

  stream.socket().shutdown(asio::ip::tcp::socket::shutdown_send, ec);
}
