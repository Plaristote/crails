#ifndef  ACTION_REQUEST_HANDLER_HPP
# define ACTION_REQUEST_HANDLER_HPP

# ifdef CRAILS_B2_INCLUDE
#  include "request_handler.hpp"
# else
#  include "../request_handler.hpp"
# endif

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
