#ifndef  REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP

# include "http_server/connection.hpp"
# include <regex>
# include <functional>
# include "multipart.hpp"

namespace Crails
{
  class Params;
  class BuildingResponse;

  class RequestParser
  {
  public:
    enum Status
    {
      Continue,
      Stop,
      Abort
    };

    virtual ~RequestParser() {}
    
    virtual void operator()(Connection&, BuildingResponse&, Params&, std::function<void(Status)>) const = 0;
  protected:
    bool content_type_matches(Params&, const std::regex) const;
  };

  class BodyParser : public RequestParser
  {
  public:
    void wait_for_body(Connection&, BuildingResponse&, Params&, std::function<void()>) const;
  protected:
    virtual void body_received(Connection&, BuildingResponse&, Params&, const std::string& body) const = 0;
  private:
    struct PendingBody
    {
      PendingBody(Connection&, BuildingResponse&, Params&);

      Connection&           connection;
      BuildingResponse&     out;
      Params&               params;
      std::string           read_buffer;
      unsigned int          total_read;
      unsigned int          to_read;
      std::function<void()> finished_callback;
    };
  };

  class RequestDataParser : public RequestParser
  {
  public:
    void operator()(Connection&, BuildingResponse&, Params&, std::function<void(RequestParser::Status)>) const override;
  private:
  };

  class RequestFormParser : public BodyParser
  {
  public:
    void operator()(Connection&, BuildingResponse&, Params&, std::function<void(RequestParser::Status)>) const override;
  private:
    void body_received(Connection&, BuildingResponse&, Params&, const std::string& body) const override;
  };

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

  class RequestJsonParser : public BodyParser
  {
  public:
   void operator()(Connection&, BuildingResponse&, Params&, std::function<void(RequestParser::Status)>) const override;
  private:
    void body_received(Connection&, BuildingResponse&, Params&, const std::string& body) const override;
  };

  class RequestXmlParser : public BodyParser
  {
  public:
   void operator()(Connection&, BuildingResponse&, Params&, std::function<void(RequestParser::Status)>) const override;
  private:
    void body_received(Connection&, BuildingResponse&, Params&, const std::string& body) const override;
  };
}

#endif
