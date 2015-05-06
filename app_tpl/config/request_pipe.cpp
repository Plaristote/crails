#include <crails/request_handlers/file.hpp>
#include <crails/request_handlers/action.hpp>

void CrailsServer::initialize_request_pipe()
{
  add_request_handler(new FileRequestHandler);
  add_request_handler(new ActionRequestHandler);
}