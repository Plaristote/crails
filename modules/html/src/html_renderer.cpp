#include <crails/renderers/html_renderer.hpp>

using namespace std;
using namespace Crails;

bool HtmlRenderer::can_render(const std::string& accept_header, const std::string& view) const
{
  if (accept_header.find("text/html") != std::string::npos
   || accept_header.find("*/*")       != std::string::npos
   || accept_header.find("text/*")    != std::string::npos)
    return templates.find(view) != templates.end();
  return (false);
}

void HtmlRenderer::render_template(const std::string& view, Data params, Data response, SharedVars& vars) const
{
  auto   tpl       = templates.find(view);
  string html_view = (*tpl).second(this, vars);
  string layout    = response["layout"].defaults_to<string>("");

  response["headers"]["Content-Type"] = "text/html";
  if (layout != "" && view != layout)
  {
    if (can_render("text/html", layout))
    {
      vars["yield"] = &html_view;
      render_template(layout, params, response, vars);
    }
    else
      throw MissingTemplate(layout);
  }
  else
    response["body"] = html_view;
}
