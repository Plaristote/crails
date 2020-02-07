#ifndef  HTML_TEMPLATE_HPP
# define HTML_TEMPLATE_HPP

# include <crails/template.hpp>

namespace Crails
{
  class HtmlTemplate : public Template
  {
  public:
    HtmlTemplate(const Renderer* renderer, SharedVars& vars) :
      Template(renderer, vars)
    {
    }
    
    static std::string html_escape(const std::string& data);
  };
}

#endif
