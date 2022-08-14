#ifndef  CRAILS_MULTIPART_PARSER_HPP
# define CRAILS_MULTIPART_PARSER_HPP

# ifdef CRAILS_B2_INCLUDE
#  include "request_parser.hpp"
# else
#  include "../request_parser.hpp"
# endif
# include "../multipart.hpp"

namespace Crails
{
  class RequestMultipartParser : public RequestParser
  {
  public:
    void operator()(Connection&, BuildingResponse&, Params&, std::function<void(RequestParser::Status)>) const override;
  private:
    void parse_multipart(Connection&, Params&, std::function<void()>) const;

    struct PendingBody
    {
      PendingBody(Connection&, Params&);

      Connection&            connection;
      Params&                params;
      MultipartParser        multipart_parser;
      std::function<void()>  finished_callback;
    };

    void on_receive(std::shared_ptr<PendingBody>, Connection&) const;
  };
}

#endif
