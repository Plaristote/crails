#ifndef  CRAILS_TESTS_REQUEST_HPP
# define CRAILS_TESTS_REQUEST_HPP

# include "../router.hpp"
# include "../params.hpp"

# define EXPECT_STATUS(request, status) \
  EXPECT(request.response["status"].as<short>(), ==, status)

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
      Request(boost::beast::http::verb method, const std::string& uri);

      void run();

      HttpRequest  request;
      HttpResponse response;
    };
  }
}

#endif
