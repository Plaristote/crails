#ifndef  ROUTER_HPP
# define ROUTER_HPP

# include <exception>
# include <functional>
# include <vector>
# include <string>
# include <iostream>

# include <crails/utils/singleton.hpp>
# include <regex>

# include "datatree.hpp"
# include "server.hpp"
# include "params.hpp"

namespace Crails
{
  class Router
  {
    Router() {}
    ~Router() {}

    SINGLETON(Router)
  public:
    typedef std::function<void (Params&,std::function<void(DataTree)>)> Action;

    struct Item
    {
      Action                   run;
      std::string              method;
      std::regex               regexp;
      std::vector<std::string> param_names;
    };

    typedef std::vector<Item> Items;

    void          initialize(void);
    const Action* get_action(const std::string& method, const std::string& uri, Params&) const;

  private:
    void      item_initialize_regex(Item& item, const std::string& route);
    void      match(const std::string& route, Action callback);
    void      match(const std::string& method, const std::string& route, Action callback);

    Items routes;
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
