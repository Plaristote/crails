#ifndef  REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP

# include "http_server.hpp"
# include <Boots/Utils/regex.hpp>

class Params;

class RequestParser
{
public:
  enum Status
  {
    Continue,
    Stop
  };
  
  virtual ~RequestParser() {}
  
  virtual Status operator()(const Server::request&, CrailsServerTraits::Response, Params&) = 0;
protected:
  bool content_type_matches(Params&, const Regex);
};

class BodyParser : public RequestParser
{
#ifdef ASYNC_SERVER  
  typedef std::function<void (boost::iterator_range<char const*>,
                            boost::system::error_code,
                            size_t,
                            Server::connection_ptr)> ReadCallback;  
  
  ReadCallback    callback;
#endif
public:
  void wait_for_body(const Server::request&, CrailsServerTraits::Response, Params&);
protected:
  virtual void body_received(const Server::request&, CrailsServerTraits::Response, Params&) = 0;
};

class RequestDataParser : public RequestParser
{
public:
  RequestParser::Status operator()(const Server::request&, CrailsServerTraits::Response, Params&);
private:
};

class RequestFormParser : public BodyParser
{
public:
  RequestParser::Status operator()(const Server::request&, CrailsServerTraits::Response, Params&);
private:
  void body_received(const Server::request&, CrailsServerTraits::Response, Params&);
};

class RequestMultipartParser : public RequestParser
{
#ifdef ASYNC_SERVER  
  typedef std::function<void (boost::iterator_range<char const*>,
                            boost::system::error_code,
                            size_t,
                            Server::connection_ptr)> ReadCallback;  
  
  ReadCallback    callback;
#endif
public:
  RequestParser::Status operator()(const Server::request&, CrailsServerTraits::Response, Params&);
private:
  void parse_multipart(const Server::request&, CrailsServerTraits::Response, Params&);
};

class RequestJsonParser : public BodyParser
{
public:
  RequestParser::Status operator()(const Server::request&, CrailsServerTraits::Response, Params&);
private:
  void body_received(const Server::request&, CrailsServerTraits::Response, Params&);
};

#endif
