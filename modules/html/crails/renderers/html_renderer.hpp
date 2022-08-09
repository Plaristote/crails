#ifndef  HTML_RENDERER_HPP
# define HTML_RENDERER_HPP

# include <crails/renderer.hpp>

namespace Crails
{
  class HtmlRenderer : public Renderer
  {
  public:
    HtmlRenderer();

    bool can_render(const std::string& accept_header, const std::string& view) const override;
    void render_template(const std::string& view, RenderTarget&, SharedVars& vars) const override;
  };
}

#endif
