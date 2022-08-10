#include "crails/http_response.hpp"
#include <algorithm>

using namespace std;
using namespace Crails;

void BuildingResponse::set_response(HttpStatus code, const string& body)
{
  set_status_code(code);
  set_body(body.c_str(), body.size());
}

void BuildingResponse::set_body(const char* str, size_t size)
{
  auto&   response = get_raw_response();
  string& out      = response.body();

  response.content_length(size);
  out.resize(size);
  std::copy(str, str + size, out.begin());
}

void BuildingResponse::send()
{
  if (!already_sent)
  {
    connection.write();
    already_sent = true;
  }
}
