#ifndef  VIEW_HPP
# define VIEW_HPP

# include <Boots/System/dynamic_library.hpp>
# include "shared_vars.hpp"

class View
{
public:
  typedef std::string (*GenerateView)(SharedVars&);

  View(const std::string& path);
  
  bool               IsValid(void) const
  {
    return (generate_view != 0);
  }

  const std::string  Generate(SharedVars& vars);
  static std::string Render(const std::string& layout_path, const std::string& view_path, SharedVars& vars);
  static bool        Compile(const std::string& view_path, std::string& backtrace);

  struct CompileException : public std::exception
  {
    virtual const char* what(void) const throw() { return (error.c_str()); }
    
    std::string error;
    std::string backtrace;
  };

private:
  DynamicLibrary view_object;
  GenerateView   generate_view;
};

extern "C"
{
  std::string render_view(const std::string& name, SharedVars& vars);
}

#endif