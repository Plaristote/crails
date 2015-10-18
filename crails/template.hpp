#ifndef  TEMPLATE_HPP
# define TEMPLATE_HPP

# include "renderer.hpp"

namespace Crails
{
  class Template
  {
  public:
    Template(const Renderer* renderer, SharedVars& vars) : renderer(renderer), vars(vars)
    {}

    std::string partial(const std::string& view);
  private:
    const Renderer* renderer;
  protected:
    SharedVars&     vars;
  };
}

#endif
