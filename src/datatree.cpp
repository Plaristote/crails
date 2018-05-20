#include <boost/property_tree/json_parser.hpp>
#include <boost/optional/optional.hpp>
#include <crails/datatree.hpp>
#include <sstream>
#include <fstream>

using namespace std;

string DataTree::to_json() const
{
  return const_cast<DataTree*>(this)->as_data().to_json();
}

DataTree& DataTree::from_json(stringstream& stream)
{
  boost::property_tree::read_json(stream, tree);
  return *this;
}

DataTree& DataTree::from_json(const string& json)
{
  stringstream stream;

  stream.str(json);
  return from_json(stream);
}

DataTree& DataTree::from_json_file(const string& json_file)
{
  ifstream stream(json_file.c_str());

  boost::property_tree::read_json(stream, tree);
  return *this;
}

void Data::each(std::function<bool (Data)> functor)
{
  auto& tree = (path == "") ? *(this->tree) : this->tree->get_child(path);

  for (boost::property_tree::ptree::value_type& v : tree)
  {
    Data data(v.second, v.first);

    data.overload_path("");
    if (!(functor(data)))
      break ;
  }
}

void Data::each(std::function<bool (const Data)> functor) const
{
  const auto& tree = (path == "") ? *(this->tree) : this->tree->get_child(path);

  for (const boost::property_tree::ptree::value_type& v : tree)
  {
    Data data(const_cast<boost::property_tree::ptree&>(v.second), v.first);

    data.overload_path("");
    if (!(functor(data)))
      break ;
  }
}

Data Data::at(unsigned int i) const
{
  if (count() <= i)
    throw boost_ext::out_of_range("Data::operator[] out of range");
  {
    auto& tree = (path == "") ? *(this->tree) : this->tree->get_child(path);
    auto  it   = tree.begin();

    std::advance(it, i);
    {
      Data data(it->second, it->first);

      data.overload_path("");
      return data;
    }
  }
}

bool Data::exists() const
{
  boost::optional< boost::property_tree::ptree& > child = tree->get_child_optional(path);

  if (child)
    return true;
  return false;
}

bool Data::is_blank() const
{
  return !exists() || as<string>() == "";
}

bool Data::is_null() const
{
  return !exists() || as<string>() == "null";
}

std::vector<std::string> Data::get_keys() const
{
  auto& tree = (path == "") ? *(this->tree) : this->tree->get_child(path);
  vector<string> keys;

  for (boost::property_tree::ptree::value_type& v : tree)
    keys.push_back(v.first);
  return keys;
}

std::vector<std::string> Data::find_missing_keys(const std::vector<std::string>& keys) const
{
  vector<string> missing_keys;
  string         path_prefix;

  if (path.size() > 0)
    path_prefix = path + '.';
  for (string key : keys)
  {
    auto child = tree->get_child_optional(path_prefix + key);

    if (!child)
      missing_keys.push_back(key);
  }
  return missing_keys;
}

bool Data::require(const std::vector<std::string>& keys) const
{
  return find_missing_keys(keys).size() == 0;
}

bool Data::is_array() const
{
  for (const auto& value : get_ptree())
  {
    if (value.first != "")
      return false;
  }
  return true;
}

void Data::merge(Data data)
{
  boost::property_tree::ptree& local_tree = get_ptree();

  if (data.is_array())
  {
    for (auto value : data.get_ptree())
      local_tree.push_back(std::make_pair("", value.second));
  }
  else
  {
    for (auto value : data.get_ptree())
      local_tree.put_child(value.first, value.second);
  }
}

void Data::merge(DataTree data_tree)
{
  merge(data_tree.as_data());
}

void Data::destroy()
{
  tree->erase(path);
  tree->get_child(context).erase(key);
}

void Data::output(std::ostream& out) const
{
  boost::property_tree::json_parser::write_json(out, get_ptree());
}

string Data::to_json() const
{
  stringstream stream;

  output(stream);
  return stream.str();
}
