#ifndef  CRAILS_JSON_RENDERER_HPP
# define CRAILS_JSON_RENDERER_HPP

# ifdef CRAILS_B2_INCLUDE
#  include "renderer.hpp"
# else
#  include "../renderer.hpp"
# endif

namespace Crails
{
  class JsonRenderer : public Renderer
  {
  public:
    JsonRenderer();

    bool can_render(const std::string& accept_header, const std::string& view) const override;
    void render_template(const std::string& view, RenderTarget& response, SharedVars& vars) const override;
  };
}

#endif
