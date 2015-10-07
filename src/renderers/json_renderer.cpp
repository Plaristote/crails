#include <crails/renderers/json_renderer.hpp>

using namespace Crails;

bool JsonRenderer::can_render(const std::string& accept_header, const std::string& view) const
{
  if (accept_header.find("/json") != std::string::npos)
    return templates.find(view) != templates.end();
  return false;
}

void JsonRenderer::render_template(const std::string& view, Data params, Data response, SharedVars& vars) const
{
  auto tpl = templates.find(view);
  std::string json_view = (*tpl).second((vars));

  response["headers"]["Content-Type"] = "application/json";
  response["body"] = json_view;
}

std::string JsonTemplate::javascript_escape(std::string val) const
{
  return val;
}

void JsonTemplate::add_separator()
{
  if (!first_item_in_object)
    stream << ',';
  first_item_in_object = false;
}

void JsonTemplate::add_key(const std::string& key)
{
  stream << '"' << javascript_escape(key) << "\":";
}

void JsonTemplate::add_object(std::function<void()> func)
{
  stream << '{';
  first_item_in_object = true;
  func();
  stream << '}';
  first_item_in_object = false;
}

namespace Crails
{
  template<>
  void JsonTemplate::add_value<std::string>(const std::string val)
  {
    stream << '"' << javascript_escape(val) << '"';
  }

  template<>
  void JsonTemplate::json<std::function<void()> >(const std::string& key, const std::function<void()> func)
  {
    add_separator();
    add_key(key);
    add_object(func);
  }
}
