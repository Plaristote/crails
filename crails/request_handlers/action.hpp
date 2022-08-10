#ifndef  ACTION_REQUEST_HANDLER_HPP
# define ACTION_REQUEST_HANDLER_HPP

# include "../request_handler.hpp"

namespace Crails
{
  class ActionRequestHandler : public RequestHandler
  {
  public:
    ActionRequestHandler(void) : RequestHandler("action") {}

    void operator()(Context&, std::function<void(bool)> callback) const override;
  private:
  };
}

#endif
