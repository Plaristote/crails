#include <crails/renderers/archive_renderer.hpp>
#include <crails/archive.hpp>

using namespace Crails;

bool ArchiveRenderer::can_render(const std::string& accept_header, const std::string& view) const
{
  if (accept_header.find(Archive::mimetype) != std::string::npos)
    return templates.find(view) != templates.end();
  return false;
}

void ArchiveRenderer::render_template(const std::string& view, Data params, Data response, SharedVars& vars) const
{
  auto tpl = templates.find(view);
  std::string body = (*tpl).second(this, vars);

  response["headers"]["Content-Type"] = Archive::mimetype;
  response["body"] = body;
}
