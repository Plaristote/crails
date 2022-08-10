#ifndef  ROUTER_HPP
# define ROUTER_HPP

# include <exception>
# include <iostream>
# include <crails/utils/singleton.hpp>
# include "router_base.hpp"
# include "context.hpp"

namespace Crails
{
  class Router : public RouterBase<Crails::Params, std::function<void (Crails::Context&, std::function<void()>)> >
  {
    Router() {}
    ~Router() {}

    SINGLETON(Router)
  public:
    void initialize(void);
  };

  template<typename CONTROLLER>
  class ActionRoute
  {
    typedef void (CONTROLLER::*Method)();
  public:
    static void trigger(Crails::Context& context, Method method, std::function<void()> callback)
    {
      auto controller = std::make_shared<CONTROLLER>(context);

      if (!context.response.sent())
      {
        controller->callback = std::bind(&ActionRoute<CONTROLLER>::finalize, controller.get(), callback);
        controller->ActionController::initialize();
        if (!context.response.sent())
          (controller.get()->*method)();
      }
      else
        callback();
      controller->close_on_deletion = true;
    }

  private:
    static void finalize(CONTROLLER* controller, std::function<void()> callback)
    {
      controller->ActionController::finalize();
      callback();
    }
  };
}

# define SYM2STRING(sym) std::string(#sym)

# define match_action(method, path, controller, action) \
  match(method, path, [](Crails::Context& context, std::function<void()> callback) \
  { \
    context.params["controller-data"]["name"]   = #controller; \
    context.params["controller-data"]["action"] = #action; \
    ActionRoute<controller>::trigger(context, &controller::action, callback); \
  })

# define crud_actions(resource_name, controller) \
  match_action("GET",    '/' + SYM2STRING(resource_name),               controller,index);  \
  match_action("GET",    '/' + SYM2STRING(resource_name) + "/:id" ,     controller,show);   \
  match_action("POST",   '/' + SYM2STRING(resource_name),               controller,create); \
  match_action("PUT",    '/' + SYM2STRING(resource_name) + "/:id",      controller,update); \
  match_action("DELETE", '/' + SYM2STRING(resource_name) + "/:id",      controller,destroy);

# define resource_actions(resource_name, controller) \
  match_action("GET",    '/' + SYM2STRING(resource_name) + "/new",      controller,new_);    \
  match_action("GET",    '/' + SYM2STRING(resource_name) + "/:id/edit", controller,edit);   \
  crud_actions(resource_name, controller)
#endif
