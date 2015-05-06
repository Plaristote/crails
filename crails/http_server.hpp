#ifndef  HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include <boost/network/protocol/http/server.hpp>

namespace http = boost::network::http;

class  BuildingResponse;
struct CrailsServer;

# ifdef ASYNC_SERVER
typedef http::async_server<CrailsServer> Server;

struct CrailsServerTraits
{
  typedef Server::connection::status_t HttpCode;
  typedef Server::connection           HttpCodes;
  typedef Server::connection_ptr       Response;
};
# else
typedef http::server<CrailsServer>       Server;

struct CrailsServerTraits
{
  typedef Server::response::status_type HttpCode;
  typedef Server::response              HttpCodes;
  typedef Server::response&             Response;
};
# endif

#endif
