#ifndef  ROUTER_HPP
# define ROUTER_HPP

# include <exception>
# include <functional>
# include <vector>
# include <string>
# include <iostream>

# include <Boots/Utils/singleton.hpp>
# include <Boots/Utils/regex.hpp>

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
    typedef std::function<DataTree (Params&)> Action;

    struct Item
    {
      Action                   run;
      std::string              method;
      Regex                    regex;
      std::vector<std::string> param_names;
    };

    typedef std::vector<Item> Items;

    void          initialize(void);
    const Action* get_action(const std::string& method, const std::string& uri, Params&) const;

  private:
    void      item_initialize_regex(Item& item, std::string route);
    void      match(const std::string& route, Action callback);
    void      match(const std::string& method, const std::string& route, Action callback);

    Items routes;
  };
}

# define SYM2STRING(sym) std::string(#sym)

# define SetRoute(method, route, klass, function) \
  match(method, route, [](Params& params) -> DataTree \
  { \
    params["controller-data"]["name"]   = #klass; \
    params["controller-data"]["action"] = #function; \
    klass controller(params); \
\
    if (controller.response["status"].exists()) \
      return (controller.response); \
    controller.initialize(); \
    if (!controller.response["status"].exists()) \
      controller.function(); \
    controller.finalize(); \
    return (controller.response); \
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
