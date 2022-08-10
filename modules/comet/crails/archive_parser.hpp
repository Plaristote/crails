#ifndef  ARCHIVE_PARSER_HPP
# define ARCHIVE_PARSER_HPP

# include <crails/request_parser.hpp>

namespace Crails
{
  class RequestArchiveParser : public BodyParser
  {
  public:
    void operator()(Connection&, BuildingResponse&, Params& params, std::function<void(RequestParser::Status)> callback);
    void body_received(Connection&, BuildingResponse&, Params& params, const std::string& body);
  };
}

#endif
