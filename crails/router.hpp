#ifndef  ROUTER_HPP
# define ROUTER_HPP

# include <exception>
# include <iostream>

# include <crails/utils/singleton.hpp>

# include "datatree.hpp"
# include "server.hpp"
# include "params.hpp"
# include "router_base.hpp"

namespace Crails
{
  class Router : public RouterBase<Crails::Params, std::function<void (Crails::Params&,std::function<void(DataTree)>)> >
  {
    Router() {}
    ~Router() {}

    SINGLETON(Router)
  public:
    void          initialize(void);
  };
}

# define SYM2STRING(sym) std::string(#sym)

# define SetAsyncRoute(method, route, klass, func) \
  match(method, route, [](Params& params, std::function<void(DataTree)> callback) \
  { \
    params["controller-data"]["name"]   = #klass; \
    params["controller-data"]["action"] = #func; \
    std::shared_ptr<klass> controller = std::make_shared<klass>(params); \
    auto                   callback_2 = [controller, callback]() \
    { \
      DataTree response = controller->response; \
\
      controller.reset(0); \
      callback(response); \
    }; \
\
    if (controller->response["status"].exists()) \
      callback_2(); \
    controller->initialize([controller, callback_2]() \
    { \
      auto callback_3 = [=](){ controller->finalize(callback_2);}; \
\
      if (!controller->response["status"].exists()) \
        controller->func(callback_3); \
      else \
        callback_3(); \
    }); \
  });

# define SetRoute(method, route, klass, func) \
  match(method, route, [](Params& params, std::function<void(DataTree)> callback) \
  { \
    DataTree response; \
\
    params["controller-data"]["name"]   = #klass; \
    params["controller-data"]["action"] = #func; \
    { \
      klass controller(params); \
\
      if (controller.response["status"].exists()) \
        response = controller.response; \
      else \
      { \
        controller.initialize(); \
        if (!controller.response["status"].exists()) \
          controller.func(); \
        controller.finalize(); \
        response = controller.response; \
      } \
    } \
    callback(response); \
  });

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
