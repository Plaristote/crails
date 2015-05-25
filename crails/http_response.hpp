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
    
    void set_response(Server::HttpCode code, const std::string& body);

    void set_status_code(Server::HttpCode code);
    void set_headers(const std::string& key, const std::string& value);
    void set_body(const char* str, size_t size);
    
    void bundle(void);

  private:
    Server::Response response;
    Headers          headers;
  };
}

#endif