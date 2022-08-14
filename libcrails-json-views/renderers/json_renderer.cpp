#include "json_renderer.hpp"

using namespace Crails;

bool JsonRenderer::can_render(const std::string& accept_header, const std::string& view) const
{
  if (accept_header.find("/json") != std::string::npos
   || accept_header.find("*/*")   != std::string::npos)
    return templates.find(view) != templates.end();
  return false;
}

void JsonRenderer::render_template(const std::string& view, RenderTarget& target, SharedVars& vars) const
{
  auto tpl = templates.find(view);
  std::string json_view = (*tpl).second(this, vars);

  target.set_header("Content-Type", "application/json");
  target.set_body(json_view.c_str(), json_view.length());
}
