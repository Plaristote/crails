#ifndef  VIEW_HPP
# define VIEW_HPP

# include <Boots/System/dynamic_library.hpp>
# include "shared_vars.hpp"
# include <functional>

namespace Crails
{
  class View
  {
  public:
    typedef std::string (*GenerateView)(SharedVars&);

    View(const std::string& path);
    
    bool               is_valid(void) const
    {
      return (generate_view != 0);
    }

    const std::string  generate(SharedVars& vars);
    static std::string render(const std::string& layout_path, const std::string& view_path, SharedVars& vars);

  private:
    DynamicLibrary view_object;
    GenerateView   generate_view;
  };
}

extern "C"
{
  std::string render_view(const std::string& name, Crails::SharedVars& vars);
  std::string render_layout(const std::string& name, Crails::SharedVars& vars, std::function<std::string ()> yield);
}

#endif