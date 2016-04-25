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
  // Set the Content-Length parameter
  {
    std::stringstream stream;

    stream << size;
    set_headers("Content-Length", stream.str());
  }
  std::string  out;
  out.resize(size);
  std::copy(str, str + size, out.begin());
  bundle();
  response->write(out);
}

void BuildingResponse::set_status_code(Server::HttpCode code)
{
  response->set_status(code);
}

void BuildingResponse::set_headers(const std::string& key, const std::string& value)
{
  Headers::iterator it = std::find(headers.begin(), headers.end(), key);
  
  if (it == headers.end())
    headers.push_back(Header(key, value));
  else
    it->value = value;
}

void BuildingResponse::bundle(void )
{
  std::function<void (Header&)>     callback;
  std::function<void (void)>        cleanup;
  unsigned short                    count       = headers.size();
  HttpServer::response_header*      arr_headers = new HttpServer::response_header[count];
  unsigned short                it          = 0;

  callback = [this, &arr_headers, &it](Header& header)
  {
    arr_headers[it].name  = header.key;
    arr_headers[it].value = header.value;
    it += 1;
  };
  cleanup = [this, &arr_headers](void)
  {
    response->set_headers(boost::make_iterator_range(arr_headers, arr_headers + headers.size()));
    delete[]   arr_headers;
  };
  std::for_each(headers.begin(), headers.end(), callback);
  cleanup();
}
