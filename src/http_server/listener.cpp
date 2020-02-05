#include "crails/http_server/listener.hpp"
#include "crails/http_server/http_session.hpp"
#include "crails/logger.hpp"
#include <boost/asio/strand.hpp>

using namespace Crails;

Listener::Listener(boost::asio::io_context& io_context, boost::asio::ip::tcp::endpoint endpoint) : io_context(io_context), acceptor(boost::asio::make_strand(io_context))
{
  boost::beast::error_code ec;

  acceptor.open(endpoint.protocol(), ec);
  if (ec)
    on_initialization_failure(ec);

  acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec)
    on_initialization_failure(ec);

  acceptor.bind(endpoint, ec);
  if (ec)
    on_initialization_failure(ec);

  acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec)
    on_initialization_failure(ec);
}

void Listener::on_initialization_failure(boost::beast::error_code ec)
{
  logger << Logger::Error << "Crails::Listener: " << ec.message() << Logger::endl;
  throw std::runtime_error("cannot start crails listener");
}

void Listener::run()
{
  wait_accept();
}

void Listener::wait_accept()
{
  acceptor.async_accept(
    boost::asio::make_strand(io_context),
    boost::beast::bind_front_handler(&Listener::on_accept, shared_from_this())
  );
}

void Listener::on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket)
{
  if (!ec)
    std::make_shared<Crails::HttpSession>(std::move(socket))->run();
  else
    logger << Logger::Error << "/!\\ Crails::Listener failed to accept a connection" << Logger::endl;
  wait_accept();
}
