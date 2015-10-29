#ifndef  DATATREE_HPP
# define DATATREE_HPP

# include <boost/property_tree/ptree.hpp>
# include <functional>
# include <iostream>

class DataTree;

class Data
{
  friend class DataTree;
protected:
  Data(boost::property_tree::ptree& tree, const std::string& key) :
    tree(tree),
    context(""),
    key(key),
    path(key)
  {
  }

  Data(boost::property_tree::ptree& tree, const std::string& context, const std::string& key) :
    tree(tree),
    context(context),
    key(key),
    path(context + '.' + key)
  {
  }

public:
  Data operator[](const std::string& key)
  {
    return Data(tree, path, key);
  }

  template<typename T>
  T operator[](const std::string& key) const
  {
    return tree.get<T>(path + '.' + key);
  }

  const std::string& get_path() const { return path; }
  const std::string& get_key()  const { return key; }

  std::size_t count() const
  {
    return tree.get_child(path).size();
  }

  template<typename T>
  T as() const { return tree.get<T>(path); }

  template<typename T>
  T defaults_to(const T def) { return tree.get(path, def); }

  template<typename T>
  operator T() const { return tree.get<T>(path); }

  template<typename T>
  Data& operator=(const T value)
  {
    tree.put(path, value);
    return *this;
  }

  template<typename T>
  bool operator==(const T value) const
  {
    return tree.get<T>(path) == value;
  }

  template<typename T>
  void push_back(const T value)
  {
    boost::property_tree::ptree child;

    child.put("", value);
    if (!(exists()))
    {
      boost::property_tree::ptree array;

      array.push_back(std::make_pair("", child));
      tree.add_child(path, array);
    }
    else
      get_ptree().push_back(std::make_pair("", child));
  }

  bool exists() const;
  void destroy() { tree.erase(path); }

  void each(std::function<void (Data)> functor);
  void _break() { _each_break = true; }

  void output(std::ostream& out = std::cout) const;
  std::string to_json() const;

  void merge(Data data);
  void merge(DataTree data_tree);

  boost::property_tree::ptree& get_ptree() { return tree.get_child(path); }
  const boost::property_tree::ptree& get_ptree() const { return tree.get_child(path); }

private:
  boost::property_tree::ptree& tree;
  const std::string            context, key, path;
  bool                         _each_break;
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
 