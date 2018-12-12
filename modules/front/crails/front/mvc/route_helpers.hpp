#ifndef  CRAILS_FRONT_ROUTE_HELPERS_HPP
# define CRAILS_FRONT_ROUTE_HELPERS_HPP

# include <crails/front/exception.hpp>

# define route(path, controller_class, method) \
  match(path, [](const Crails::Front::Params& params) \
  { \
    auto* controller = new controller_class(params); \
    \
    controller->initialize().then([controller]() \
    { \
      controller->method().then([controller]() \
      { \
        controller->finalize().then([controller]() { delete controller; }) \
                              ._catch([controller]() { delete controller; Crails::raise(std::runtime_error(std::string("Router cannot finalize ") + #controller_class + "::" + #method)); }); \
      })._catch([controller]() \
      { \
        delete controller; \
        Crails::raise(std::runtime_error(std::string("Controller action failed: ") + #controller_class + "::" + #method)); \
      }); \
    })._catch([controller]() \
    { \
      delete controller; \
      Crails::raise(std::runtime_error(std::string("Router cannot initialize ") + #controller_class + "::" + #method)); \
    }); \
  });

#endif
