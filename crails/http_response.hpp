#ifndef  HTTP_RESPONSE
# define HTTP_RESPONSE

# include "server.hpp"

class BuildingResponse
{
public:
  struct Header
  {
    Header(std::string key, std::string value) : key(key), value(value)
    {}

    bool    operator==(const std::string& comp) const { return (key == comp); }
    Header& operator=(const std::string& new_val)     { value = new_val; return (*this); }

    std::string key;
    std::string value;
  };

  typedef std::list<Header> Headers;

  BuildingResponse(CrailsServer::Response response) : response(response)
  {}
  
  void SetResponse(CrailsServer::HttpCode code, const std::string& body);

  void SetStatusCode(CrailsServer::HttpCode code);
  void SetHeaders(const std::string& key, const std::string& value);
  void SetBody(const char* str, size_t size);
  
  void Bundle(void);

private:
  CrailsServer::Response response;
  Headers                headers;
};

#endif