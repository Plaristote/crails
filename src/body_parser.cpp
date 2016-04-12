#include "crails/server.hpp"
#include "crails/params.hpp"
#include <crails/logger.hpp>
#include <condition_variable>

using namespace std;
using namespace Crails;

#ifdef ASYNC_SERVER
using namespace std::chrono;

typedef std::function<void (boost::iterator_range<char const*>,
                            boost::system::error_code,
                            size_t,
                            HttpServer::connection_ptr)> ReadCallback;

void BodyParser::wait_for_body(const HttpServer::request& request, BuildingResponse& out, Params& params)
{
  shared_ptr<int>    state = make_shared<int>(0);
  ReadCallback       callback;
  mutex              sem_mutex;
  unique_lock<mutex> sem_lock(sem_mutex);
  condition_variable sem;
  auto&              response   = out.get_response();
  unsigned int       to_read    = params["headers"]["Content-Length"].defaults_to<unsigned int>(0);
  unsigned int       total_read = 0;
  std::string        read_buffer;
  Regex              get_boundary;
  
  callback = [this, state, &callback, &sem, &to_read, &total_read, &read_buffer, &response](boost::iterator_range<char const*> range,
                        boost::system::error_code error_code,
                        size_t size_read,
                        HttpServer::connection_ptr connection_ptr)
  {
    if (*state == -1)
    {
      *state = 1;
      for (unsigned int i = 0 ; i < size_read ; ++i)
        read_buffer += range[i];
      total_read += size_read;
      if (total_read == to_read)
        sem.notify_all();
      else
        response->read(callback);
    }
  };
  response->read(callback);
  if (sem.wait_until(sem_lock, system_clock::now() + seconds(10)) == cv_status::timeout)
  {
    const char* error = *state == 1 ? "client request timed out" : "no threads available to take care of the request";

    throw std::runtime_error(error);
  }
  body_received(request, out, params, read_buffer);
}
#else
void BodyParser::wait_for_body(const HttpServer::request& request, BuildingResponse& out, Params& params)
{
  body_received(request, out, params, request.body);
}
#endif
