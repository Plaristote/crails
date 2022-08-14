#include "template.hpp"
#include "renderer.hpp"

using namespace Crails;
using namespace std;

std::string Template::partial(const std::string& view, SharedVars vars_)
{
  auto templates = renderer->get_templates();
  auto template_ = templates.find(view);

  if (template_ == templates.end())
    throw MissingTemplate(view);
  else
  {
    SharedVars duplicate = vars;

    for (auto& var : vars_)
      duplicate[var.first] = var.second;
    return (*template_).second(renderer, duplicate);
  }
}
