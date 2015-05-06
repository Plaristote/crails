#include <Boots/System/process.hpp>
#include <unistd.h>
#include "crails/view.hpp"
#include "crails/platform.hpp"
#include <iostream>

using namespace std;
using namespace System;
using namespace Crails;

View::View(const std::string& path)
{
  std::string full_path;

  full_path  = "../";
  full_path += "app/views/" + path + '.' + DYNLIB_EXT;
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

std::string render_layout(const std::string& name, SharedVars& vars, std::function<std::string ()> yield)
{
    std::string* old_yield = (std::string*)(*vars["yield"]);
    std::string html, yield_html;

    yield_html       = yield();
    *vars["yield"]   = &yield_html;
    html             = render_view(name, vars);
    *vars["yield"]   = old_yield;
    return (html);
}
