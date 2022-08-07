#include "crails/http_response.hpp"
#include <algorithm>

using namespace std;
using namespace Crails;

void BuildingResponse::set_response(Server::HttpCode code, const string& body)
{
  set_status_code(code);
  set_body(body.c_str(), body.size());
}

void BuildingResponse::set_body(const char* str, size_t size)
{
  auto&   response = get_response();
  string& out      = response.body();

  response.content_length(size);
  out.resize(size);
  std::copy(str, str + size, out.begin());
  send();
}

void BuildingResponse::set_status_code(Server::HttpCode code)
{
  get_response().result(code);
}

void BuildingResponse::set_headers(const std::string& key, const std::string& value)
{
  headers.emplace(key, value);
}

void BuildingResponse::send()
{
  for (auto it = headers.cbegin() ; it != headers.cend() ; ++it)
    get_response().set(it->first, it->second);
  connection.write();
}
