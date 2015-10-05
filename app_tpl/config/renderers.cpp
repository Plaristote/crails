#include <crails/renderer.hpp>
#include <crails/renderers/html_renderer.hpp>

using namespace Crails;

void Renderer::initialize()
{
  renderers.push_back(new HtmlRenderer);
}
