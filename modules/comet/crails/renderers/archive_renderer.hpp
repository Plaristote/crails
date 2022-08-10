#ifndef  CRAILS_ARCHIVE_RENDERER_HPP
# define CRAILS_ARCHIVE_RENDERER_HPP

# include <crails/renderer.hpp>

namespace Crails
{
  class ArchiveRenderer : public Renderer
  {
  public:
    ArchiveRenderer();

    bool can_render(const std::string& accept_header, const std::string& view) const;
    void render_template(const std::string& view, RenderTarget& response, SharedVars& vars) const;
  };
}

#endif

