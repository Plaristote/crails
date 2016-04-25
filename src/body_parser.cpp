#include "crails/server.hpp"
#include "crails/params.hpp"
#include <crails/logger.hpp>

using namespace std;
using namespace Crails;

BodyParser::PendingBody::PendingBody(const HttpServer::request& r, BuildingResponse& o, Params& p)
  : request(r), out(o), params(p), total_read(0)
{
  to_read = params["headers"]["Content-Length"].defaults_to<unsigned int>(0);
}

void BodyParser::on_receive(shared_ptr<PendingBody> pending_body,
                    boost::iterator_range<char const*> range,
                    size_t size_read)
{
  for (unsigned int i = 0 ; i < size_read ; ++i)
    pending_body->read_buffer += range[i];
  pending_body->total_read += size_read;
  if (pending_body->total_read == pending_body->to_read)
  {
    body_received(pending_body->request, pending_body->out, pending_body->params, pending_body->read_buffer);
    pending_body->finished_callback();
  }
  else
  {
    pending_body->out.get_response()->read([this, pending_body](boost::iterator_range<char const*> range, std::error_code, size_t size_read, HttpServer::connection_ptr)
    {
      on_receive(pending_body, range, size_read);
    });
  }
}

void BodyParser::wait_for_body(const HttpServer::request& request, BuildingResponse& out, Params& params, function<void()> finished_callback)
{
  shared_ptr<PendingBody> pending_body = make_shared<PendingBody>(request, out, params);

  pending_body->finished_callback = finished_callback;
  out.get_response()->read([this, pending_body](boost::iterator_range<char const*> range, std::error_code, size_t size_read, HttpServer::connection_ptr)
  {
    on_receive(pending_body, range, size_read);
  });
}
