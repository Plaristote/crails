#ifndef  JSON_RENDERER_HPP
# define JSON_RENDERER_HPP

# include <crails/renderer.hpp>

namespace Crails
{
  class JsonRenderer : public Renderer
  {
  public:
    JsonRenderer();

    bool can_render(const std::string& accept_header, const std::string& view) const;
    void render_template(const std::string& view, Data params, Data response, SharedVars& vars) const;
  };
}

#endif
