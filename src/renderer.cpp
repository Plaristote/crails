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
