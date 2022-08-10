#include <boost/asio/dispatch.hpp>
#include "crails/http_server/connection.hpp"
#include "crails/logger.hpp"
#include "crails/context.hpp"

using namespace Crails;
using namespace boost;

Connection::Connection(const Server& server_, HttpRequest request) :
  server(server_),
  stream(std::move(asio::ip::tcp::socket(server.get_io_context()))),
  request(request)
{}

Connection::Connection(const Server& server_, asio::ip::tcp::socket socket_) :
  server(server_),
  stream(std::move(socket_))
{
  logger << Logger::Info << "Crails::Connection opened" << Logger::endl;
}

Connection::~Connection()
{
  logger << Logger::Info << "Crails::Connection closed" << Logger::endl;
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
    std::make_shared<Context>(server, *this)->run();
  else if (ec != boost::asio::error::eof && ec != boost::asio::error::timed_out)
    logger << Logger::Error << "!! Crails failed to read on socket: " << ec.message() << Logger::endl;
}

void Connection::write()
{
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
