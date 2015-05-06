#ifndef  HTTP_RESPONSE
# define HTTP_RESPONSE

# include "server.hpp"

namespace Crails
{
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

    BuildingResponse(Server::Response response) : response(response)
    {}
    
    void SetResponse(Server::HttpCode code, const std::string& body);

    void SetStatusCode(Server::HttpCode code);
    void SetHeaders(const std::string& key, const std::string& value);
    void SetBody(const char* str, size_t size);
    
    void Bundle(void);

  private:
    Server::Response response;
    Headers          headers;
  };
}

#endif