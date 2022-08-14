#ifndef  CRAILS_RENDER_TARGET_HPP
# define CRAILS_RENDER_TARGET_HPP

# include <string>
# include <string_view>

namespace Crails
{
  class RenderTarget
  {
  public:
    virtual void set_header(const std::string&, const std::string&) {}
    virtual void set_body(const char* str, size_t size) = 0;
  };

  class RenderString : public RenderTarget
  {
  public:
    void set_body(const char* str, size_t size) { body = std::string_view(str, size); }
    std::string_view value() const { return body; }
  private:
    std::string_view body;
  };
}

#endif
