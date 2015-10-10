#include <crails/request_handlers/file.hpp>
#include <crails/request_handlers/action.hpp>
#include <crails/request_parser.hpp>

using namespace Crails;

const Logger::Symbol Logger::log_level = Info;

void Server::initialize_request_pipe()
{
  add_request_parser(new RequestDataParser);
  add_request_parser(new RequestFormParser);
  add_request_parser(new RequestJsonParser);
  add_request_parser(new RequestMultipartParser);

  add_request_handler(new FileRequestHandler);
  add_request_handler(new ActionRequestHandler);
}
