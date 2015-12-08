#include "crails/http_response.hpp"
#include <algorithm>

using namespace std;
using namespace Crails;

#ifndef ASYNC_SERVER
static void SetHeaderParameter(Server::Response response, const std::string& key, const std::string& value)
{
  auto it  = response.headers.begin();
  auto end = response.headers.end();
  
  for (; it != end ; ++it)
  {
    boost::network::http::response_header_narrow& header = *it;

    if (header.name == key)
    {
      header.value = value;
      return ;
    }
  }
  response.headers.push_back({key,value});
}
#endif

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
#ifdef ASYNC_SERVER
  std::string  out;
#else
  std::string& out = response.content;
#endif
  out.resize(size);
  std::copy(str, str + size, out.begin());
  bundle();
#ifdef ASYNC_SERVER
  response->write(out);
#endif
}

void BuildingResponse::set_status_code(Server::HttpCode code)
{
#ifdef ASYNC_SERVER
  response->set_status(code);
#else
  response = HttpServer::response::stock_reply(code);
#endif
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
#ifdef ASYNC_SERVER
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
#else
  callback = [this](Header& header)
  {
    SetHeaderParameter(response, header.key, header.value);
  };
  cleanup  = [](void) {};
#endif
  std::for_each(headers.begin(), headers.end(), callback);
  cleanup();
}
