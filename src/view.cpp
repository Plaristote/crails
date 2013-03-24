#include <Boots/System/process.hpp>
#include <unistd.h>
#include "crails/view.hpp"

using namespace std;
using namespace System;

View::View(const std::string& path)
{
#ifdef SERVER_DEBUG
  // Views are automatically compiled in debug, if deemed necessary
  std::string backtrace;
  
  if (!(Compile(path, backtrace)))
  {
    CompileException exc;

    exc.error     = "Failed to compile view '" + path + '\'';
    exc.backtrace = backtrace;
    throw exc;
  }
#endif
  std::string full_path;

  full_path  = "../";
  full_path += "app/views/" + path + ".so";
  if (view_object.Load(full_path))
    generate_view = view_object.Resolve<GenerateView>("generate_view");
  else
    generate_view = 0;
}

const std::string View::Generate(SharedVars& vars)
{
  if (generate_view != 0)
    return (generate_view(vars));
  return ("[View::Error] Could not load the view's dynamically loaded library '" + view_object.Filename() + "'<br />" + view_object.Error());
}

std::string       View::Render(const std::string& layout_path, const std::string& view_path, SharedVars& vars)
{
  View        layout(layout_path);
  View        view(view_path);
  std::string html_view   = view.Generate(vars);

  *(vars["yield"]) = &html_view;
  return (layout.Generate(vars));
}

bool              View::Compile(const std::string& path, std::string& backtrace)
{
  char tmp[256];
  std::string relative_path = "../app/views/" + path;
  std::string output_path   = relative_path + ".so";
  std::string current_path  = getcwd(tmp, 256);
  Process     compile_view("/usr/bin/ruby", { current_path + "/../scripts/compile_view.rb", relative_path });
  
  if (compile_view.Execute())
  {

    if (compile_view.Join() != 0)
    {
      vector<string> compile_args = { "-Wall", "-g", "-fPIC", "-shared", "-I..", "-std=c++11" };
      
      compile_args.push_back(relative_path + ".cpp");
      compile_args.push_back("-o");
      compile_args.push_back(output_path);
      {
        Process compile_lib("/usr/bin/g++", compile_args);
        
        if (compile_lib.Execute())
        {
          if (compile_lib.Join() != 0)
          {
            Process      rm_tmp("/usr/bin/rm", { relative_path + ".cpp" });
            stringstream output;

            rm_tmp.Execute();
            compile_lib.Stderr(output);
            backtrace = output.str();            
            return (false);
          }
          return (true);
        }
      }
    }
    return (true);
  }
  return (false);
}

/*
 * Render Partial Function
 */
std::string render_view(const std::string& name, SharedVars& vars)
{
  View view(name);
  
  if (view.IsValid())
    return (view.Generate(vars));
  return ("[Crails][Error] couldn't render view '" + name + "'<br />\n");
}
