#ifndef  CRAILS_DATA_PARSER_HPP
# define CRAILS_DATA_PARSER_HPP

# ifdef CRAILS_B2_INCLUDE
#  include "request_parser.hpp"
# else
#  include "../request_parser.hpp"
# endif

namespace Crails
{
  class RequestUrlParser : public RequestParser
  {
  public:
    void operator()(Connection&, BuildingResponse&, Params&, std::function<void(RequestParser::Status)>) const override;
  };
}

#endif
