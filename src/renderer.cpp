#include <crails/renderer.hpp>
#include <iostream>
using namespace Crails;
using namespace std;

list<Renderer*> Crails::renderers;
thread_local Renderer::Context Renderer::current_context;

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
    {
      Context new_context(renderer, params, response, vars);

      renderer->render_template(view, params, response, vars);
    }
  }
}

std::string Renderer::partial(const std::string& view)
{
  std::string format = current_context.params["headers"]["Accept"].Value();

  if (current_context.renderer != 0 && current_context.renderer->can_render(format, view))
  {
    current_context.renderer->render_template(
      view,
      current_context.params,
      current_context.response,
      *current_context.vars);
    return current_context.response["body"].Value();
  }
  return "";
}

Renderer::Context::Context(Renderer* renderer, Data params, Data response, SharedVars& vars) :
  renderer(renderer), params(params), response(response), vars(&vars)
{
  Renderer::current_context = *this;
}

Renderer::Context::~Context()
{
  Renderer::current_context.renderer = 0;
}

const Renderer::Context& Renderer::Context::operator=(const Context& copy)
{
  renderer = copy.renderer;
  params = copy.params;
  response = copy.response;
  vars = copy.vars;
  return *this;
}

