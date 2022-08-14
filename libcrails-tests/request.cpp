#include "request.hpp"
#include "helper.hpp"

using namespace std;
using namespace Crails;

extern Server* test_server;

Tests::Request::Request(boost::beast::http::verb method, const std::string& uri)
{
  request.method(method);
  request.target(uri);
}

void Tests::Request::run()
{
  const Router* router = Router::singleton::Get();

  if (router)
  {
    auto stub_connection = make_shared<Crails::Connection>(*test_server, request);
    auto stub_request    = make_shared<Crails::Context>(*test_server, *stub_connection);

    stub_request->run();
    if (!stub_request->handled)
      throw RouteNotFound(std::string(boost::beast::http::to_string(request.method())) + '#' + std::string(request.target()));
  }
  else
    throw RouterNotInitialized();
}
