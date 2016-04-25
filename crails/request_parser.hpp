#ifndef  REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP

# include "http_server.hpp"
# include <crails/utils/regex.hpp>
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
    
    virtual void operator()(const HttpServer::request&, BuildingResponse&, Params&, std::function<void(Status)>) = 0;
  protected:
    bool content_type_matches(Params&, const Regex);
  };

  class BodyParser : public RequestParser
  {
  public:
    void wait_for_body(const HttpServer::request&, BuildingResponse&, Params&, std::function<void()>);
  protected:
    virtual void body_received(const HttpServer::request&, BuildingResponse&, Params&, const std::string& body) = 0;
  private:
    struct PendingBody
    {
      PendingBody(const HttpServer::request&, BuildingResponse&, Params&);

      const HttpServer::request& request;
      BuildingResponse&          out;
      Params&                    params;
      std::string                read_buffer;
      unsigned int               total_read;
      unsigned int               to_read;
      std::function<void()>      finished_callback;
    };

    void on_receive(std::shared_ptr<PendingBody> pending_body,
                    boost::iterator_range<char const*> range,
                    size_t size_read);
  };

  class RequestDataParser : public RequestParser
  {
  public:
    void operator()(const HttpServer::request&, BuildingResponse&, Params&, std::function<void(RequestParser::Status)>);
  private:
  };

  class RequestFormParser : public BodyParser
  {
  public:
    void operator()(const HttpServer::request&, BuildingResponse&, Params&, std::function<void(RequestParser::Status)>);
  private:
    void body_received(const HttpServer::request&, BuildingResponse&, Params&, const std::string& body);
  };

  class RequestMultipartParser : public RequestParser
  {
  public:
    void operator()(const HttpServer::request&, BuildingResponse&, Params&, std::function<void(RequestParser::Status)>);
  private:
    void parse_multipart(const HttpServer::request&, ServerTraits::Response, Params&, std::function<void()>);
    struct PendingBody
    {
      PendingBody(ServerTraits::Response r, Params& p);

      ServerTraits::Response response;
      Params&                params;
      MultipartParser        multipart_parser;
      std::function<void()>  finished_callback;
    };

    void on_receive(std::shared_ptr<PendingBody> pending_body,
                    boost::iterator_range<char const*> range,
                    size_t size_read,
                    HttpServer::connection_ptr);
  };

  class RequestJsonParser : public BodyParser
  {
  public:
   void operator()(const HttpServer::request&, BuildingResponse&, Params&, std::function<void(RequestParser::Status)>);
  private:
    void body_received(const HttpServer::request&, BuildingResponse&, Params&, const std::string& body);
  };
}

#endif
