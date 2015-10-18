#include <crails/json_template.hpp>

using namespace Crails;

std::string JsonTemplate::javascript_escape(std::string val) const
{
  // TODO implement javascript_escape
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
