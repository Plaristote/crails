#ifndef  HTTP_RESPONSE
# define HTTP_RESPONSE

# include "server/connection.hpp"
# include "render_target.hpp"
# include <unordered_map>

namespace Crails
{
  class BuildingResponse : public RenderTarget
  {
  public:
    BuildingResponse(Connection& connection) : connection(connection)
    {}

    void set_response(HttpStatus code, const std::string& body);
    void set_status_code(HttpStatus code) { get_raw_response().result(code); }
    void set_header(boost::beast::http::field key, const std::string& value) { get_raw_response().set(key, value); }
    void set_header(const std::string& key, const std::string& value) override { get_raw_response().set(key, value); }
    void set_body(const char* str, size_t size) override;
    void send();
    bool sent() const { return already_sent; }

    HttpStatus get_status_code() const { return get_raw_response().result(); }
    HttpResponse& get_raw_response() { return connection.get_response(); }
    const HttpResponse& get_raw_response() const { return connection.get_response(); }

  private:
    Connection& connection;
    bool        already_sent = false;
  };
}

#endif
