#ifndef  ARCHIVE_PARSER_HPP
# define ARCHIVE_PARSER_HPP

# include <crails/request_parser.hpp>

namespace Crails
{
  class RequestArchiveParser : public BodyParser
  {
  public:
    void operator()(const HttpServer::request& request, BuildingResponse& out, Params& params, std::function<void(RequestParser::Status)> callback);
    void body_received(const HttpServer::request&, BuildingResponse&, Params& params, const std::string& body);
  };
}

#endif
