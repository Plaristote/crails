#ifndef  CRAILS_RENDER_TARGET_HPP
# define CRAILS_RENDER_TARGET_HPP

# include <string>

namespace Crails
{
  class RenderTarget
  {
  public:
    virtual void set_headers(const std::string& key, const std::string& value) = 0;
    virtual void set_body(const char* str, size_t size) = 0;
  };
}

#endif
