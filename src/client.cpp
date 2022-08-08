#include "crails/client.hpp"
#include "crails/server.hpp"
#include <boost/lexical_cast.hpp>
#include "crails/logger.hpp"

using namespace std;
using namespace Crails;

Client::Client(const std::string& host, unsigned short port) :
  host(host), port(port),
  resolver(Crails::Server::get_io_context()),
  stream(Crails::Server::get_io_context())
{
}

Client::~Client()
{
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both);
}

void Client::connect()
{
  const auto results = resolver.resolve(host, boost::lexical_cast<std::string>(port));

  stream.connect(results);
}

HttpResponse Client::query(const HttpRequest& request)
{
  HttpResponse              response;
  boost::beast::flat_buffer buffer;

  boost::beast::http::write(stream, request);
  boost::beast::http::read(stream, buffer, response);
  return response;
}

void Client::async_query(const HttpRequest& request, function<void(const HttpResponse&)> callback)
{
  boost::beast::http::async_write(stream, request, [this, callback](boost::beast::error_code ec, size_t)
  {
    logger << Logger::Info << "Client::async_write done" << Logger::endl;
    auto buffer = make_shared<boost::beast::flat_buffer>();
    auto response = make_shared<HttpResponse>();

    if (ec)
    {
      logger << Logger::Error << "Crails::Client: request write failed: " << ec.message() << Logger::endl;
      return ;
    }
    boost::beast::http::async_read(stream, *buffer, *response, [callback, buffer, response](boost::beast::error_code ec, size_t)
    {
      if (ec)
        logger << Logger::Error << "Crails::Client: request read failed: " << ec.message() << Logger::endl;
      else
        callback(*response);
    });
  });
}
