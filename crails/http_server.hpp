#ifndef  HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include <boost/network/protocol/http/server.hpp>

namespace Crails
{
  namespace http = boost::network::http;

  class BuildingResponse;
  class Server;

  typedef http::server<Server>               HttpServer;

  struct ServerTraits
  {
    typedef HttpServer::connection::status_t HttpCode;
    typedef HttpServer::connection           HttpCodes;
    typedef HttpServer::connection_ptr       Response;
  };
}

#endif
