#ifndef  ROUTER_HPP
# define ROUTER_HPP

# include <exception>
# include <iostream>

# include <crails/utils/singleton.hpp>

# include "datatree.hpp"
# include "server.hpp"
# include "params.hpp"
# include "router_base.hpp"
# include "request.hpp"

namespace Crails
{
  class Router : public RouterBase<Crails::Params, std::function<void (Crails::Request&, std::function<void()>)> >
  {
    Router() {}
    ~Router() {}

    SINGLETON(Router)
  public:
    void initialize(void);
  };

  template<typename CONTROLLER>
  class ControllerRoute
  {
    typedef void (CONTROLLER::*Method)();
  public:
    static void trigger(Crails::Request& request, Method method, std::function<void()> callback)
    {
      auto controller = std::make_shared<CONTROLLER>(request);

      if (!request.out.sent())
      {
        controller->callback = std::bind(&ControllerRoute<CONTROLLER>::finalize, controller.get(), callback);
        controller->initialize();
        if (!request.out.sent())
          (controller.get()->*method)();
      }
      else
        callback();
    }

  private:
    static void finalize(CONTROLLER* controller, std::function<void()> callback)
    {
      controller->finalize();
      callback();
    }
  };
}

# define SYM2STRING(sym) std::string(#sym)

# define SetRoute(method, route, klass, func) \
  match(method, route, [](Crails::Request& request, std::function<void()> callback) \
  { \
    request.params["controller-data"]["name"]   = #klass; \
    request.params["controller-data"]["action"] = #func; \
    ControllerRoute<klass>::trigger(request, &klass::func, callback); \
  })

# define match_action(method, path, controller, action) SetRoute(method, path, controller, action)

# define Crudify(resource_name, controller) \
  SetRoute("GET",    '/' + SYM2STRING(resource_name),               controller,index);  \
  SetRoute("GET",    '/' + SYM2STRING(resource_name) + "/:id" ,     controller,show);   \
  SetRoute("POST",   '/' + SYM2STRING(resource_name),               controller,create); \
  SetRoute("PUT",    '/' + SYM2STRING(resource_name) + "/:id",      controller,update); \
  SetRoute("DELETE", '/' + SYM2STRING(resource_name) + "/:id",      controller,destroy);

# define SetResource(resource_name, controller) \
  SetRoute("GET",    '/' + SYM2STRING(resource_name) + "/new",      controller,new);    \
  SetRoute("GET",    '/' + SYM2STRING(resource_name) + "/:id/edit", controller,edit);   \
  Crudify(resource_name, controller)

#endif
