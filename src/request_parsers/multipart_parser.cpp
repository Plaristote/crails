#include "crails/server.hpp"
#include "crails/params.hpp"
#include "crails/multipart.hpp"

using namespace std;
using namespace Crails;

RequestParser::Status RequestMultipartParser::operator()(const HttpServer::request& request, ServerTraits::Response response, Params& params)
{
  static const Regex is_multipart("^multipart/form-data", REG_EXTENDED);

  if (params["method"].Value() != "GET" && content_type_matches(params, is_multipart))
  {
    parse_multipart(request, response, params);
    return RequestParser::Stop;
  }
  return RequestParser::Continue;
}

#ifdef ASYNC_SERVER
void RequestMultipartParser::parse_multipart(const HttpServer::request&, ServerTraits::Response response, Params& params)
{
  cout << "Going for multipart/form-data parsing" << endl;
  MultipartParser multipart_parser;

  multipart_parser.Initialize(params);
  callback = [this, &multipart_parser, &params](boost::iterator_range<char const*> range,
                      boost::system::error_code error_code,
                      size_t size_read,
                      HttpServer::connection_ptr connection_ptr)
  {
    multipart_parser.sem.Wait();
    multipart_parser.total_read += size_read;
    for (unsigned int i = 0 ; i < size_read ; ++i)
      multipart_parser.read_buffer += range[i];
    multipart_parser.Parse(params);
    if (multipart_parser.total_read < multipart_parser.to_read)
      connection_ptr->read(callback);
    else
      params.response_parsed.Post();
    multipart_parser.sem.Post();
  };
  response->read(callback);
  params.response_parsed.Wait();
}
#else
void RequestMultipartParser::parse_multipart(const HttpServer::request& request, ServerTraits::Response, Params& params)
{
  MultipartParser multipart_parser;

  cout << "[" << request.method << " " << request.destination << "] Going for multipart/form-data parsing" << endl;
  multipart_parser.Initialize(params);
  multipart_parser.read_buffer = request.body;
  multipart_parser.Parse(params);
  params.response_parsed.Post();
}
#endif