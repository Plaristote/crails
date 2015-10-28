#include <boost/property_tree/json_parser.hpp>
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
  _each_break = false;
  for (boost::property_tree::ptree::value_type& v : tree.get_child(path))
  {
    functor(Data(v.second, v.first));
    if (_each_break)
      break ;
  }
}

bool Data::exists() const
{
  if (tree.count(context) > 0)
  {
    for (boost::property_tree::ptree::value_type& v : tree.get_child(context))
    {
      if (v.first == key)
        return true;
    }
  }
  return false;
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
