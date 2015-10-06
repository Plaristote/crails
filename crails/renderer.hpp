#ifndef  RENDERER_HPP
# define RENDERER_HPP

# include <map>
# include <Boots/Utils/datatree.hpp>
# include "shared_vars.hpp"

namespace Crails
{
  class Renderer
  {
  protected:
    typedef std::string (*Generator)(SharedVars&);
    typedef std::map<std::string, Generator> Templates;

  public:
    static void initialize();
    static void finalize();

    virtual ~Renderer() {}
    virtual bool can_render(const std::string& accept_header, const std::string& view) const = 0;
    virtual void render_template(const std::string& view, Data params, Data response, SharedVars& vars) const = 0;

    static void render(const std::string& view, Data params, Data response, SharedVars& vars);

  protected:
    Templates templates;
  };

  extern std::list<Renderer*> renderers;
}

#endif
