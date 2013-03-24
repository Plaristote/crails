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

# define ROUTER_RESSOURCE(resource_name, controller) \
  Match("GET",    '/' + SYM2STRING(resource_name),               controller::index); \
  Match("GET",    '/' + SYM2STRING(resource_name) + "/new",      controller::new); \
  Match("GET",    '/' + SYM2STRING(resource_name) + "/:id/edit", controller::edit); \
  Match("GET",    '/' + SYM2STRING(resource_name) + "/:id" ,     controller::show); \
  Match("POST",   '/' + SYM2STRING(resource_name),               controller::create); \
  Match("PUT",    '/' + SYM2STRING(resource_name) + "/:id",      controller::update); \
  Match("DELETE", '/' + SYM2STRING(resource_name) + "/:id",      controller::delete);
  
# define SetRoute(method, route, klass, function) \
  Match(method, route, [](Params& params) -> DynStruct { return (klass::RescueFrom(klass::function, params)); });

#endif