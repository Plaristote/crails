#ifndef  ROUTER_HPP
# define ROUTER_HPP

# include <exception>
# include <functional>
# include <vector>
# include <string>
# include <Boots/Utils/dynstruct.hpp>
# include <iostream>

# include <Boots/Utils/singleton.hpp>
# include <Boots/Utils/regex.hpp>

# include "server.hpp"
# include "params.hpp"

class Router
{
  Router() {}
  ~Router() {}
  
  SINGLETON(Router)
public:
  typedef std::function<DynStruct (Params&)> Callback;
  
  class Exception404
  {
  };
  
  struct Exception302
  {
    std::string redirect_to;
  };

  struct Item
  {
    Callback                 run;
    std::string              method;
    Regex                    regex;
    std::vector<std::string> param_names;
  };

  typedef std::vector<Item> Items;

  void      Initialize(void);
  DynStruct Execute(const std::string& method, const std::string& uri, Params& params) const;

private:
  void      ItemInitializeRegex(Item& item, std::string route);
  void      Match(const std::string& route, Callback callback);
  void      Match(const std::string& method, const std::string& route, Callback callback);

  Items routes;
};

# define SYM2STRING(sym) std::string(#sym)

# define SetRoute(method, route, klass, function) \
  Match(method, route, [](Params& params) -> DynStruct \
  { \
    klass controller(params); \
\
    return (klass::RescueFrom([&controller]() -> DynStruct { return (controller.function()); })); \
  });

# define SetResource(resource_name, controller) \
  SetRoute("GET",    '/' + SYM2STRING(resource_name),               controller,Index);  \
  SetRoute("GET",    '/' + SYM2STRING(resource_name) + "/new",      controller,New);    \
  SetRoute("GET",    '/' + SYM2STRING(resource_name) + "/:id/edit", controller,Edit);   \
  SetRoute("GET",    '/' + SYM2STRING(resource_name) + "/:id" ,     controller,Show);   \
  SetRoute("POST",   '/' + SYM2STRING(resource_name),               controller,Create); \
  SetRoute("PUT",    '/' + SYM2STRING(resource_name) + "/:id",      controller,Update); \
  SetRoute("DELETE", '/' + SYM2STRING(resource_name) + "/:id",      controller,Delete);

#endif