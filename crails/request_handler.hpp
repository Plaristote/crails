#ifndef  REQUEST_HANDLER_HPP
# define REQUEST_HANDLER_HPP

# include "context.hpp"

namespace Crails
{
  class RequestHandler
  {
  public:
    RequestHandler(const std::string& name) : name(name) {}
    virtual ~RequestHandler() {}

    const std::string& get_name(void) const { return name; }

    virtual void operator()(Context&, std::function<void(bool)> callback) const = 0;
    
  private:
    const std::string name;
  };
}

#endif
