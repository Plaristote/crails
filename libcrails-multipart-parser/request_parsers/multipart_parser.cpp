#include "server/connection.hpp"
#include "params.hpp"
#include "logger.hpp"
#include "multipart_parser.hpp"

using namespace std;
using namespace Crails;

void RequestMultipartParser::operator()(Connection& connection, BuildingResponse&, Params& params, function<void(RequestParser::Status)> callback) const
{
  static const regex is_multipart("^multipart/form-data", regex_constants::extended);

  if (params["method"].as<string>() != "GET" && content_type_matches(params, is_multipart))
  {
    parse_multipart(connection, params, [callback]()
    {
      callback(RequestParser::Stop);
    });
  }
  else
    callback(RequestParser::Continue);
}

RequestMultipartParser::PendingBody::PendingBody(Connection& c, Params& p)
  : connection(c), params(p)
{
  multipart_parser.initialize(params);
}

void RequestMultipartParser::on_receive(shared_ptr<PendingBody> pending_body, Connection& connection) const
{
  MultipartParser& multipart_parser = pending_body->multipart_parser;
  const string& body = connection.get_request().body();
  unsigned int offset = multipart_parser.read_buffer.length();

  multipart_parser.total_read = body.length();
  for (unsigned int i = 0 ; i < body.length() - offset ; ++i)
    multipart_parser.read_buffer += body[i + offset];
  multipart_parser.parse(pending_body->params);
  if (multipart_parser.total_read < multipart_parser.to_read)
    throw std::runtime_error("RequestMutlipartParser: Asynchronous body reception not implemented");
  else
    pending_body->finished_callback();
}

void RequestMultipartParser::parse_multipart(Connection& connection, Params& params, function<void()> finished_callback) const
{
  shared_ptr<PendingBody> pending_body = make_shared<PendingBody>(connection, params);

  pending_body->finished_callback = finished_callback;
  on_receive(pending_body, connection);
}
