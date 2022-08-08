#ifndef  ACTION_REQUEST_HANDLER_HPP
# define ACTION_REQUEST_HANDLER_HPP

# include "../request_handler.hpp"

namespace Crails
{
  class ActionRequestHandler : public RequestHandler
  {
  public:
    ActionRequestHandler(void) : RequestHandler("action") {}

    void operator()(Request& request, std::function<void(bool)> callback) override;
  private:
  };
}

#endif
