#ifndef  REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP

# include "http_server.hpp"
# include <crails/utils/regex.hpp>

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
    
    virtual Status operator()(const HttpServer::request&, BuildingResponse&, Params&) = 0;
  protected:
    bool content_type_matches(Params&, const Regex);
  };

  class BodyParser : public RequestParser
  {
  #ifdef ASYNC_SERVER  
    typedef std::function<void (boost::iterator_range<char const*>,
                              boost::system::error_code,
                              size_t,
                              HttpServer::connection_ptr)> ReadCallback;  
    
    ReadCallback    callback;
  #endif
  public:
    void wait_for_body(const HttpServer::request&, BuildingResponse&, Params&);
  protected:
    virtual void body_received(const HttpServer::request&, BuildingResponse&, Params&, const std::string& body) = 0;
  };

  class RequestDataParser : public RequestParser
  {
  public:
    RequestParser::Status operator()(const HttpServer::request&, BuildingResponse&, Params&);
  private:
  };

  class RequestFormParser : public BodyParser
  {
  public:
    RequestParser::Status operator()(const HttpServer::request&, BuildingResponse&, Params&);
  private:
    void body_received(const HttpServer::request&, BuildingResponse&, Params&, const std::string& body);
  };

  class RequestMultipartParser : public RequestParser
  {
  #ifdef ASYNC_SERVER  
    typedef std::function<void (boost::iterator_range<char const*>,
                              boost::system::error_code,
                              size_t,
                              HttpServer::connection_ptr)> ReadCallback;  
    
    ReadCallback    callback;
  #endif
  public:
    RequestParser::Status operator()(const HttpServer::request&, BuildingResponse&, Params&);
  private:
    void parse_multipart(const HttpServer::request&, ServerTraits::Response, Params&);
  };

  class RequestJsonParser : public BodyParser
  {
  public:
    RequestParser::Status operator()(const HttpServer::request&, BuildingResponse&, Params&);
  private:
    void body_received(const HttpServer::request&, BuildingResponse&, Params&, const std::string& body);
  };
}

#endif
