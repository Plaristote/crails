#include "request_parser.hpp"
#include "params.hpp"
#include "logger.hpp"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace Crails;

BodyParser::PendingBody::PendingBody(Connection& c, BuildingResponse& o, Params& p)
  : connection(c), out(o), params(p), total_read(0)
{
  const char* sizeHeader = "Content-Length";
  if (c.get_request().find(sizeHeader) == c.get_request().end())
    to_read = 0;
  else
    to_read = boost::lexical_cast<unsigned int>(c.get_request()[sizeHeader]);
  total_read = c.get_request().body().size();
}

void BodyParser::wait_for_body(Connection& connection, BuildingResponse& out, Params& params, function<void()> finished_callback) const
{
  shared_ptr<PendingBody> pending_body = make_shared<PendingBody>(connection, out, params);

  if (pending_body->total_read >= pending_body->to_read)
    finished_callback();
  else
    throw std::runtime_error("BoddyParser: Asynchronous body reception not implemented");
}
