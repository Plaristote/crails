#include <boost/property_tree/json_parser.hpp>
#include <boost/optional/optional.hpp>
#include <crails/datatree.hpp>
#include <sstream>
#include <fstream>

using namespace std;

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

void Data::each(std::function<void (Data)> functor)
{
  auto& tree = (path == "") ? *(this->tree) : this->tree->get_child(path);

  _each_break = false;
  for (boost::property_tree::ptree::value_type& v : tree)
  {
    Data data(v.second, v.first);

    data.overload_path("");
    functor(data);
    if (_each_break)
      break ;
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
  return !exists();
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

void Data::merge(Data data)
{
  boost::property_tree::ptree& local_tree = get_ptree();

  for (auto value : data.get_ptree())
    local_tree.put_child(value.first, value.second);
}

void Data::merge(DataTree data_tree)
{
  merge(data_tree.as_data());
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
