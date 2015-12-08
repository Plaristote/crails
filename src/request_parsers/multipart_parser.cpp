#include "crails/server.hpp"
#include "crails/params.hpp"
#include "crails/multipart.hpp"
#include "crails/logger.hpp"

using namespace std;
using namespace Crails;

RequestParser::Status RequestMultipartParser::operator()(const HttpServer::request& request, BuildingResponse& out, Params& params)
{
  static const Regex is_multipart("^multipart/form-data", REG_EXTENDED);

  if (params["method"].as<string>() != "GET" && content_type_matches(params, is_multipart))
  {
    parse_multipart(request, out.get_response(), params);
    return RequestParser::Stop;
  }
  return RequestParser::Continue;
}

#ifdef ASYNC_SERVER
void RequestMultipartParser::parse_multipart(const HttpServer::request&, ServerTraits::Response response, Params& params)
{
  logger << Logger::Debug << "Going for multipart/form-data parsing" << Logger::endl;
  MultipartParser    multipart_parser;
  mutex              multipart_mutex;
  unique_lock<mutex> lock(multipart_mutex);

  multipart_parser.initialize(params);
  callback = [this, &multipart_parser, &params](boost::iterator_range<char const*> range,
                      boost::system::error_code error_code,
                      size_t size_read,
                      HttpServer::connection_ptr connection_ptr)
  {
    lock_guard<mutex> lock(multipart_parser.mutex);

    multipart_parser.total_read += size_read;
    for (unsigned int i = 0 ; i < size_read ; ++i)
      multipart_parser.read_buffer += range[i];
    multipart_parser.parse(params);
    if (multipart_parser.total_read < multipart_parser.to_read)
      connection_ptr->read(callback);
    else
      params.response_parsed.notify_all();
  };
  response->read(callback);
  params.response_parsed.wait(lock);
}
#else
void RequestMultipartParser::parse_multipart(const HttpServer::request& request, ServerTraits::Response, Params& params)
{
  MultipartParser multipart_parser;

  logger << Logger::Debug << "[" << request.method << " " << request.destination << "] Going for multipart/form-data parsing" << Logger::endl;
  multipart_parser.initialize(params);
  multipart_parser.read_buffer = request.body;
  multipart_parser.parse(params);
  params.response_parsed.notify_all();
}
#endif
