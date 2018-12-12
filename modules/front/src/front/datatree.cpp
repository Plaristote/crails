#include <crails/front/datatree.hpp>
#include <crails/front/globals.hpp>
#include <iostream>

using namespace std;

DataTree& DataTree::from_json(stringstream& stream)
{
  return from_json(stream.str());
}

DataTree& DataTree::from_json(const string& str)
{
  Crails::Front::String js_str(str);

  tree = Crails::Front::Object::from_json(*js_str);
  return *this;
}

DataTree& DataTree::from_json_file(const string&)
{
  __asm__("throw 'DataTree::from_json_file not available for crails-front'");
  return *this;
}

string DataTree::to_json() const
{
  return tree.to_json();
}

void Data::each(function<bool (Data)> functor)
{
  for (auto _key : get_keys())
  {
    Data data(as_object(), _key);

    if (!(functor(data)))
      break ;
  }
}

void Data::each(function<bool (const Data)> functor) const
{
  for (auto _key : get_keys())
  {
    const Data data(as_object(), _key);

    if (!(functor(data)))
      break ;
  }
}

Data Data::at(unsigned int i) const
{
  std::stringstream stream;
  std::string stringified;

  stream << i;
  stream >> stringified;
  return Data(as_object(), stringified);
}

bool Data::exists() const
{
  if (key.length())
  {
    Crails::Front::String js_key(key);

    return object->hasOwnProperty(*js_key);
  }
  return true;
}

bool Data::is_blank() const
{
  return !exists() || as<string>() == "";
}

bool Data::is_null() const
{
  return !exists() || as<string>() == "null";
}

vector<string> Data::get_keys() const
{
  return Crails::Front::window["Object"].apply("keys", as_object());
}

vector<string> Data::find_missing_keys(const vector<string>& keys) const
{
  vector<string> missing_keys;
  string         path_prefix;

  for (string key : keys)
  {
    Crails::Front::String js_key(key);

    if (!(as_object()->hasOwnProperty(*js_key)))
      missing_keys.push_back(key);
  }
  return missing_keys;
}

bool Data::require(const vector<string>& keys) const
{
  return find_missing_keys(keys).size() == 0;
}

bool Data::is_array() const
{
  return *(as_object()["constructor"]) == *(Crails::Front::window["Array"]);
}

void Data::merge(Data data)
{
  if (data.exists())
  {
    if (key.length() > 0)
    {
     if (data.is_array())
        object.set(key, data.as_object().apply("slice", 0));
      else
        object.set(key, Crails::Front::Object::from_json(data.as_object().to_json()));
    }
    else
    {
      for (const auto& key : data.get_keys())
        (*this)[key].merge(data[key]);
    }
  }
}

void Data::merge(DataTree datatree)
{
  merge(datatree.as_data());
}

void Data::output(ostream& out) const
{
  out << to_json();
}

string Data::to_json() const
{
  return as_object().to_json();
}

string Data::to_xml() const
{
  __asm__("throw 'Data::to_xml not implemented'");
  return "";
}

size_t Data::count() const
{
  if (is_array())
    return as_object()["length"];
  return get_keys().size();
}

void Data::destroy()
{
  if (key.length() > 0)
    object.unset(key);
  else
  {
    for (auto subkey : get_keys())
      object.unset(subkey);
  }
}

template<>
bool Data::as<bool>() const
{
  std::string as_string = as_object().apply("toString");
  std::stringstream stream;
  int numerical_value;

  if (as_string == "true" || as_string == "false")
    return as_string == "true";
  stream << as_string;
  stream >> numerical_value;
  return numerical_value != 0;
}
