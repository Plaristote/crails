#include "crails/server.hpp"
#include "crails/params.hpp"
#include "crails/multipart.hpp"
#include "crails/logger.hpp"

using namespace std;
using namespace Crails;

void RequestMultipartParser::operator()(const HttpServer::request& request, BuildingResponse& out, Params& params, function<void(RequestParser::Status)> callback)
{
  static const Regex is_multipart("^multipart/form-data", REG_EXTENDED);

  if (params["method"].as<string>() != "GET" && content_type_matches(params, is_multipart))
  {
    parse_multipart(request, out.get_response(), params, [callback]()
    {
      callback(RequestParser::Stop);
    });
  }
  else
    callback(RequestParser::Continue);
}

RequestMultipartParser::PendingBody::PendingBody(ServerTraits::Response r, Params& p)
  : response(r), params(p)
{
  multipart_parser.initialize(params);
}

void RequestMultipartParser::on_receive(shared_ptr<PendingBody> pending_body, boost::iterator_range<char const*> range, size_t size_read, HttpServer::connection_ptr connection_ptr)
{
  MultipartParser& multipart_parser = pending_body->multipart_parser;

  multipart_parser.total_read += size_read;
  for (unsigned int i = 0 ; i < size_read ; ++i)
    multipart_parser.read_buffer += range[i];
  multipart_parser.parse(pending_body->params);
  if (multipart_parser.total_read < multipart_parser.to_read)
  {
    connection_ptr->read([this, pending_body](boost::iterator_range<char const*> range, boost::system::error_code, size_t size_read, HttpServer::connection_ptr connection_ptr)
    {
      on_receive(pending_body, range, size_read, connection_ptr);
    });
  }
  else
    pending_body->finished_callback();
}

void RequestMultipartParser::parse_multipart(const HttpServer::request&, ServerTraits::Response response, Params& params, function<void()> finished_callback)
{
  std::shared_ptr<PendingBody> pending_body = std::make_shared<PendingBody>(response, params);

  pending_body->finished_callback = finished_callback;
  response->read([this, pending_body](boost::iterator_range<char const*> range, boost::system::error_code, size_t size_read, HttpServer::connection_ptr connection_ptr)
  {
    on_receive(pending_body, range, size_read, connection_ptr);
  });
}
