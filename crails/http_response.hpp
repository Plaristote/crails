#ifndef  HTTP_RESPONSE
# define HTTP_RESPONSE

# include "http_server/types.hpp"
# include "http_server/connection.hpp"
# include <unordered_map>

namespace Crails
{
  class BuildingResponse
  {
  public:
    typedef std::unordered_map<std::string, std::string> Headers;

    BuildingResponse(Connection& connection) : connection(connection)
    {}

    void set_response(HttpStatus code, const std::string& body);
    void set_status_code(HttpStatus code);
    void set_headers(const std::string& key, const std::string& value);
    void set_body(const char* str, size_t size);
    void send();

    HttpResponse& get_response() { return connection.get_response(); }
    const HttpResponse& get_response() const { return connection.get_response(); }

  private:
    Connection& connection;
    Headers     headers;
  };

  class Params;

  void render_error_view(BuildingResponse& out, HttpStatus code, Params& params);
}

#endif
