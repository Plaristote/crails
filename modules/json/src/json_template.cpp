#include "crails/json_template.hpp"

using namespace Crails;

void JsonTemplate::json_array(const std::string& key, Data value)
{
  add_separator();
  add_key(key);
  stream << '[';
  first_item_in_object = true;
  value.each([this](Data item)
  {
    if (first_item_in_object == false)
      stream << ',';
    stream << item.to_json();
    first_item_in_object = false;
  });
  stream << ']';
  first_item_in_object = false;
}

std::string JsonTemplate::javascript_escape(const std::string& input) const
{
  std::string output;

  output.reserve(input.length());
  for (std::string::size_type i = 0; i < input.length(); ++i)
  {
    switch (input[i])
    {
      case '"':  output += "\\\""; break;
      case '/':  output += "\\/";  break;
      case '\b': output += "\\b";  break;
      case '\f': output += "\\f";  break;
      case '\n': output += "\\n";  break;
      case '\r': output += "\\r";  break;
      case '\t': output += "\\t";  break;
      case '\\': output += "\\\\"; break;
      default:
        output += input[i];
        break;
    }
  }
  return output;
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

void JsonTemplate::add_key(unsigned long number)
{
  stream << '"' << number << "\":";
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
  void JsonTemplate::add_value<const char*>(const char* val)
  {
    stream << '"' << javascript_escape(val) << '"';
  }

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

  template<>
  void JsonTemplate::add_value<bool>(bool value)
  {
    if (value)
      stream << "true";
    else
      stream << "false";
  }

  template<>
  void JsonTemplate::add_value<Data>(const Data value)
  {
    value.output(stream);
  }
}
