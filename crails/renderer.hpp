#ifndef  RENDERER_HPP
# define RENDERER_HPP

# include <list>
# include <map>
# include <Boots/Utils/backtrace.hpp>
# include "shared_vars.hpp"
# include "datatree.hpp"

namespace Crails
{
  struct MissingTemplate : public boost_ext::exception
  {
    MissingTemplate(const std::string& name) : name(name) {}
    const char* what() const throw() { return std::string("Template not found: '" + name + "'").c_str(); }
    std::string name;
  };

  class Template;
  
  class Renderer
  {
    friend class Template;
  protected:
    typedef std::string (*Generator)(const Renderer*, SharedVars&);
    typedef std::map<std::string, Generator> Templates;

  public:
    static void initialize();
    static void finalize();

    virtual ~Renderer() {}
    virtual bool can_render(const std::string& accept_header, const std::string& view) const = 0;
    virtual void render_template(const std::string& view, Data params, Data response, SharedVars& vars) const = 0;

    static void render(const std::string& view, Data params, Data response, SharedVars& vars);
    static std::string partial(const std::string& view);

  protected:
    Templates templates;
  };

  extern std::list<Renderer*> renderers;
}

#endif
