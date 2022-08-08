#ifndef  CRAILS_HTTP_CLIENT_HPP
# define CRAILS_HTTP_CLIENT_HPP

# include <boost/asio/ip/tcp.hpp>
# include <boost/beast/core.hpp>
# include <memory>
# include <string>
# include "crails/http_server/types.hpp"

namespace Crails
{
  class Client : public std::enable_shared_from_this<Client>
  {
    typedef boost::asio::ip::tcp::resolver Resolver;
  public:
    Client(const std::string& host, unsigned short port);
    virtual ~Client();

    void         connect();
    HttpResponse query(const HttpRequest& request);
    void         async_query(const HttpRequest& request, std::function<void(const HttpResponse&)> callback);

  private:
    const std::string        host;
    const unsigned short     port;
    Resolver                 resolver;
    boost::beast::tcp_stream stream;
  };
}

#endif
