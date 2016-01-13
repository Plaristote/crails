#ifndef  DATATREE_HPP
# define DATATREE_HPP

# include <boost/property_tree/ptree.hpp>
# include <functional>
# include <vector>
# include <string>
# include <iostream>
# include <crails/utils/backtrace.hpp>

class DataTree;

class Data
{
  friend class DataTree;

  void overload_path(const std::string& path) { this->path = path; }
protected:
  Data(boost::property_tree::ptree& tree, const std::string& key) :
    tree(&tree),
    context(""),
    key(key),
    path(key)
  {
  }

  Data(boost::property_tree::ptree& tree, const std::string& context, const std::string& key) :
    tree(&tree),
    context(context),
    key(key),
    path(context.size() > 0 ? (context + '.' + key) : key)
  {
  }

public:
  template<typename T>
  T operator[](const std::string& key) const
  {
    if (key.length() == 0)
      throw boost_ext::invalid_argument("Data::operator[] cannot take an empty string");
    return tree->get<T>(path + '.' + key);
  }

  Data operator[](const std::string& key) const
  {
    if (key.length() == 0)
      throw boost_ext::invalid_argument("Data::operator[] cannot take an empty string");
    return Data(*tree, path, key);
  }

  std::vector<std::string> find_missing_keys(const std::vector<std::string>& keys) const;
  bool                     require(const std::vector<std::string>& keys) const;

  const std::string& get_path() const { return path; }
  const std::string& get_key()  const { return key; }

  std::size_t count() const
  {
    return tree->get_child(path).size();
  }

  template<typename T>
  T as() const { return tree->get<T>(path); }

  template<typename T>
  T defaults_to(const T def) const { return tree->get(path, def); }

  template<typename T>
  operator T() const { return tree->get<T>(path); }

  template<typename T>
  operator std::vector<T>() const
  {
    std::vector<T> array;
    auto& tree = (path == "") ? *(this->tree) : this->tree->get_child(path);

    for (boost::property_tree::ptree::value_type& v : tree)
      array.push_back(v.second.get<T>(v.first));
    return array;
  }

  template<typename T>
  Data& operator=(const T value)
  {
    tree->put(path, value);
    return *this;
  }

  Data& operator=(const Data& copy)
  {
    tree    = copy.tree;
    key     = copy.key;
    context = copy.context;
    path    = copy.path;
    return *this;
  }

  template<typename T>
  bool operator==(const T value) const
  {
    return tree->get<T>(path) == value;
  }

  template<typename T>
  bool operator!=(const T value) const { return !(Data::operator==(value)); }

  Data operator||(Data value) const
  {
    return exists() ? *this : value;
  }
  
  template<typename T>
  T operator||(const T value) const
  {
    return defaults_to<T>(value);
  }

  void push_back(Data data)
  {
    if (!(exists()))
    {
      boost::property_tree::ptree array;

      array.push_back(std::make_pair("", data.get_ptree()));
      tree->add_child(path, array);
    }
    else
      get_ptree().push_back(std::make_pair("", data.get_ptree()));
  }

  template<typename T>
  void push_back(const T value)
  {
    boost::property_tree::ptree child;

    child.put("", value);
    push_back(Data(child, ""));
  }

  bool is_blank() const;
  bool exists() const;
  void destroy() { tree->erase(path); }

  void each(std::function<void (Data)> functor);
  void _break() { _each_break = true; }

  void output(std::ostream& out = std::cout) const;
  std::string to_json() const;

  void merge(Data data);
  void merge(DataTree data_tree);

  boost::property_tree::ptree& get_ptree() { return tree->get_child(path); }
  const boost::property_tree::ptree& get_ptree() const { return tree->get_child(path); }

private:
  boost::property_tree::ptree* tree;
  std::string context, key, path;
  bool        _each_break;
};

class DataTree
{
public:
  Data as_data()       { return Data(tree, "");  }
  Data operator[](const std::string& key) { return Data(tree, key); }

  DataTree& from_json(std::stringstream&);
  DataTree& from_json(const std::string&);
  DataTree& from_json_file(const std::string&);

  boost::property_tree::ptree&       get_ptree()       { return tree; }
  const boost::property_tree::ptree& get_ptree() const { return tree; }

private:
  boost::property_tree::ptree tree;
};

#endif
 
