#include <crails/renderers/html_renderer.hpp>

using namespace Crails;

bool HtmlRenderer::can_render(const std::string& accept_header, const std::string& view) const
{
  if (accept_header.find("text/html") != std::string::npos)
    return templates.find(view) != templates.end();
  return (false);
}

//void HtmlRenderer::render_template(const std::string& view, Data params, Data response, SharedVars& vars) const

