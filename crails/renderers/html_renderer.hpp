#ifndef  HTML_RENDERER_HPP
# define HTML_RENDERER_HPP

# include "../renderer.hpp"

namespace Crails
{
  class HtmlRenderer : public Renderer
  {
    typedef std::string (*GenerateView)(SharedVars&);
    typedef std::map<std::string, GenerateView> Templates;
  public:
    HtmlRenderer();

    bool can_render(const std::string& accept_header, const std::string& view) const;
    void render(const std::string& view, Data params, Data response, SharedVars& vars) const;
  };
}

#endif
