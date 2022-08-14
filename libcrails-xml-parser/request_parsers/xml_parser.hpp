#ifndef  CRAILS_XML_PARSER_HPP
# define CRAILS_XML_PARSER_HPP

# ifdef CRAILS_B2_INCLUDE
#  include "request_parser.hpp"
# else
#  include "../request_parser.hpp"
# endif

namespace Crails
{
  class RequestXmlParser : public BodyParser
  {
  public:
   void operator()(Connection&, BuildingResponse&, Params&, std::function<void(RequestParser::Status)>) const override;
  private:
    void body_received(Connection&, BuildingResponse&, Params&, const std::string& body) const override;
  };
}

#endif
