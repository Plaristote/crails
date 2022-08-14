#ifndef  CRAILS_HTML_RENDERER_HPP
# define CRAILS_HTML_RENDERER_HPP

# ifdef CRAILS_B2_INCLUDE
#  include "renderer.hpp"
# else
#  include "../renderer.hpp"
# endif

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
