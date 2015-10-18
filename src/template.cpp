#include <crails/template.hpp>

using namespace Crails;
using namespace std;

std::string Template::partial(const std::string& view)
{
  auto tpl = renderer->templates.find(view);

  if (tpl == renderer->templates.end())
    throw MissingTemplate(view);
  return (*tpl).second(renderer, vars);
}