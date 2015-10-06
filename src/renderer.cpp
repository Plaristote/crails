#include <crails/renderer.hpp>

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
  std::string format = params["headers"]["Accept"].Value();

  for (auto it = renderers.begin() ; it != renderers.end() ; ++it)
  {
    Renderer* renderer = *it;

    if (renderer->can_render(format, view))
      renderer->render_template(view, params, response, vars);
  }
}
