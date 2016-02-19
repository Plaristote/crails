#include <crails/template.hpp>

using namespace Crails;
using namespace std;

std::string Template::partial(const std::string& view, SharedVars vars)
{
  auto tpl = renderer->templates.find(view);

  if (tpl == renderer->templates.end())
    throw MissingTemplate(view);
  else
  {
    SharedVars duplicate = vars;

    for (auto& var : vars)
      duplicate[var.first] = var.second;
    return (*tpl).second(renderer, duplicate);
  }
}
