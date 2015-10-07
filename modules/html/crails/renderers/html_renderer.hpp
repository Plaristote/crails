#ifndef  HTML_RENDERER_HPP
# define HTML_RENDERER_HPP

# include <crails/renderer.hpp>

namespace Crails
{
  class HtmlRenderer : public Renderer
  {
  public:
    HtmlRenderer();

    bool can_render(const std::string& accept_header, const std::string& view) const;
    void render_template(const std::string& view, Data params, Data response, SharedVars& vars) const
    {
      auto tpl  = templates.find(view);
      std::string html_view = (*tpl).second((vars));

      response["headers"]["Content-Type"] = "text/html";
      if (response["layout"].NotNil() && view != response["layout"].Value())
      {
	*(vars["yield"]) = &html_view;
	render(response["layout"], params, response, vars);
      }
      else
	response["body"] = html_view;
    }    
  };
}

#endif
