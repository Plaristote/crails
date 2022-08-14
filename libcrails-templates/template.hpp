#ifndef  TEMPLATE_HPP
# define TEMPLATE_HPP

# include "shared_vars.hpp"
# include <string>

namespace Crails
{
  class Renderer;

  class Template
  {
  public:
    Template(const Renderer* renderer, SharedVars& vars) : renderer(renderer), vars(vars)
    {}

    std::string partial(const std::string& view, SharedVars vars = {});
  protected:
    SharedVars&     vars;
  private:
    const Renderer* renderer;
  };
}

#endif
