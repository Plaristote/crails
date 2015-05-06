#ifndef  ACTION_REQUEST_HANDLER_HPP
# define ACTION_REQUEST_HANDLER_HPP

# include "../server.hpp"

namespace Crails
{
  class ActionRequestHandler : public RequestHandler
  {
  public:
    ActionRequestHandler(void) : RequestHandler("action") {}

    bool operator()(const HttpServer::request& request, BuildingResponse& out, Params& params);
  private:
  };
}

#endif