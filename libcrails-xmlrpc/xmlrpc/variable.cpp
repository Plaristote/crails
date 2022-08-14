#include "variable.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace XmlRpc;

const Variable& Variable::operator[](const char* name) const
{
  return members.at(name);
}

const Variable& Variable::operator[](const std::string& name) const
{
  return operator[](name.c_str());
}

const Variable& Variable::operator[](unsigned int i) const
{
  auto it = array.begin();

  if (array.size() <= i)
  {
    std::stringstream stream;
    stream << "XmlRpc::Variable(array): asked for index " << i << ", size is " << array.size();
    throw std::out_of_range(stream.str());
  }
  std::advance(it, i);
  return *it;
}

Variable Variable::from_date(const std::time_t value)
{
  Variable variable;
  auto ptime = boost::posix_time::from_time_t(value);

  variable.name  = "dateTime.iso8601";
  variable.value = boost::posix_time::to_iso_extended_string(ptime) + 'Z';
  return variable;
}

Variable Variable::from_data(const Data data_value)
{
  Variable   variable;
  const Data data = data_value.at(0);

  variable.name  = data.get_key();
  if (variable.name == "struct")
    variable.from_struct(data);
  else if (variable.name == "array")
    variable.from_array(data);
  else
    variable.value = data.as<std::string>();
  return variable;
}

void Variable::from_struct(const Data data)
{
  data.each([this](const Data member) -> bool
  {
    Variable value   = Variable::from_data(member["value"]);
    std::string name = member["name"];

    members.emplace(name, value);
    return true;
  });
}

void Variable::from_array(const Data data)
{
  data["data"].each([this](const Data item) -> bool
  {
    array.push_back(Variable::from_data(item));
    return true;
  });
}

Variable::Variable() {}

Variable::Variable(int number)
{
  name  = "int";
  value = boost::lexical_cast<std::string>(number);
}

Variable::Variable(double number)
{
  name  = "double";
  value = boost::lexical_cast<std::string>(number);
}

Variable::Variable(bool boolean)
{
  name  = "boolean";
  value = boolean ? '1' : '0';
}

Variable::Variable(const std::string& str)
{
  name  = "string";
  value = str;
}

void Variable::validate_type_or_raise(const std::string& type) const
{
  if (name != type)
    throw std::runtime_error("invalid cast: " + name + " to " + type);
}

bool Variable::is_nil() const
{
  return name == "nil";
}

int Variable::as_int() const
{
  if (name != "int" && name != "i4")
    throw std::runtime_error("invalid cast: " + name + " to int");
  return boost::lexical_cast<int>(value);
}

double Variable::as_double() const
{
  if (name != "double")
    throw std::runtime_error("invalid cast: " + name + " to double");
  return boost::lexical_cast<double>(value);
}

bool Variable::as_boolean() const
{
  if (name != "boolean")
    throw std::runtime_error("invalid cast: " + name + " to boolean");
  return value[0] == '1';
}

const std::string& Variable::as_string() const
{
  if (name != "string")
    throw std::runtime_error("invalid cast: " + name + " to string");
  return value;
}

std::time_t Variable::as_date() const
{
  if (name != "dateTime.iso8601")
    throw std::runtime_error("invalid cast: " + name + " to dateTime.iso8601");
  auto ptime = boost::posix_time::from_iso_string(value);

  return boost::posix_time::to_time_t(ptime);
}

void Variable::to_xml(std::stringstream& stream) const
{
  stream << '<' << name << '>';
  if (name == "array")
  {
    stream << "<data>";
    for (const auto& variable : array)
    {
      stream << "<value>";
      variable.to_xml(stream);
      stream << "</value>";
    }
    stream << "</data>";
  }
  else if (name == "struct")
  {
    for (const auto& entry : members)
    {
      stream << "<member>"
             << "<name>" << entry.first << "</name>"
             << "<value>";
      entry.second.to_xml(stream);
      stream << "</value></member>";
    }
  }
  else
    stream << value;
  stream << "</" << name << '>';
}
