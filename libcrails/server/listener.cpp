#include <boost/asio/strand.hpp>
#include "listener.hpp"
#include "connection.hpp"
#include "../logger.hpp"
#include "../server.hpp"

using namespace Crails;
using namespace boost;

Listener::Listener(const Server& server) :
  server(server),
  acceptor(asio::make_strand(server.get_io_context())),
  socket{server.get_io_context()}
{
}

bool Listener::listen(asio::ip::tcp::endpoint endpoint, beast::error_code& ec)
{
  acceptor.open(endpoint.protocol(), ec);
  if (ec) return false;
  acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec) return false;
  acceptor.bind(endpoint);
  if (ec) return false;
  acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
  return !ec;
}

void Listener::run()
{
  wait_accept();
}

void Listener::wait_accept()
{
  acceptor.async_accept(
    socket,
    beast::bind_front_handler(&Listener::on_accept, shared_from_this())
  );
}

void Listener::on_accept(boost::beast::error_code ec)
{
  if (!ec)
    std::make_shared<Connection>(server, std::move(socket))->start();
  else
    logger << Logger::Info << "Crails::Listener failed to accept a connection: " << ec.message() << Logger::endl;
  wait_accept();
}
