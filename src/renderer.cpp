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
  std::string format   = params["headers"]["Accept"].Value();
  Renderer*   renderer = NULL;

  for (auto it = renderers.begin() ; it != renderers.end() ; ++it)
  {
    if ((*it)->can_render(format, view))
    {
      renderer = *it;
      break ;
    }
  }
  if (renderer == NULL)
    throw MissingTemplate(view);
  renderer->render_template(view, params, response, vars);
}
