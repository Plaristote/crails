#include <crails/renderer.hpp>
#include <iostream>
using namespace Crails;
using namespace std;

list<Renderer*> Crails::renderers;

void Renderer::finalize()
{
  for (auto it = renderers.begin() ; it != renderers.end() ; ++it)
    delete *it;
  renderers.clear();
}

void Renderer::render(const std::string& view, Data params, Data response, SharedVars& vars)
{
  const Renderer* renderer = pick_renderer(view, params);

  if (renderer == NULL)
    throw MissingTemplate(view);
  renderer->render_template(view, params, response, vars);
}

Renderer* Renderer::pick_renderer(const std::string& view, Data params)
{
  string format = params["headers"]["Accept"].defaults_to<string>(default_format);

  for (auto it = renderers.begin() ; it != renderers.end() ; ++it)
  {
    if ((*it)->can_render(format, view))
      return *it;
  }
  return NULL;
}

bool Renderer::can_render(const std::string& view, Data params)
{
  return pick_renderer(view, params) != NULL;
}
