#ifndef  CRAILS_TESTS_REQUEST_HPP
# define CRAILS_TESTS_REQUEST_HPP

# include "../router.hpp"
# include "../params.hpp"

namespace Crails
{
  namespace Tests
  {
    class Request
    {
      struct RouterNotInitialized : public std::exception
      {
        const char* what() const throw() { return "Router not initialized"; }
      };

      struct RouteNotFound : public std::exception
      {
        RouteNotFound(const std::string& route) : message("Route not found: " + route) {}
        const char* what() const throw() { return message.c_str(); }
        const std::string message;
      };

    public:
      Request(const std::string& method, const std::string& uri);

      void run();

      Params    params;
      DynStruct response;
    private:
    };
  }
}

#endif
