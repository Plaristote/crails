#ifndef  XMLRPC_STRUCT_HPP
# define XMLRPC_STRUCT_HPP

# include "variable.hpp"

// TODO implement more of these ?
# define XmlRpc_string_property(name) \
  const std::string& get_##name() const { return data[#name].as_string(); } \
  void set_##name(const std::string& value) { data.members[#name] = XmlRpc::Variable(value); } \
  bool is_##name##_nil() const { return data[#name].is_nil(); }

# define XmlRpc_int_property(name) \
  int get_##name() const { return data[#name].as_int(); } \
  void set_##name(int value) { data.members[#name] = XmlRpc::Variable(value); } \
  bool is_##name##_nil() const { return data[#name].is_nil(); }

# define XmlRpc_double_property(name) \
  double get_##name() const { return data[#name].as_double(); } \
  void set_##name(double value) { data.members[#name] = XmlRpc::Variable(value) } \
  bool is_##name##_nil() const { return data[#name].is_nil(); }

# define XmlRpc_date_property(name) \
  std::time_t get_##name() const { return data[#name].as_date(); } \
  void set_##name(std::time_t value) { data.members[#name] = XmlRpc::Variable::from_date(value); } \
  bool is_##name##_nil() const { return data[#name].is_nil(); }

# define XmlRpc_bool_property(name) \
  bool get_##name() const { return data[#name].as_boolean(); } \
  void set_##name(bool value) { data.members[#name] = XmlRpc::Variable(value); } \
  bool is_##name##_nil() const { return data[#name].is_nil(); }

# define XmlRpc_string_array_property(name) \
  std::vector<std::string> get_##name() const { return data[#name].as_array<std::string>(&XmlRpc::Variable::as_string); } \
  void set_##name(const std::vector<std::string>& value) { data.members[#name] = XmlRpc::Variable(value); } \
  bool is_##name##_nil() const { return data[#name].is_nil(); }

# define XmlRpc_int_array_property(name) \
  std::vector<int> get_##name() const { return data[#name].as_array<int>(&XmlRpc::Variable::as_int); } \
  void set_##name(const std::vector<int>& value) { data.members[#name] = XmlRpc::Variable(value); } \
  bool is_##name##_nil() const { return data[#name].is_nil(); }

# define XmlRpc_double_array_property(name) \
  std::vector<double> get_##name() const { return data[#name].as_array<std::string>(&XmlRpc::Variable::as_double); } \
  void set_##name(const std::vector<double>& value) { data.members[#name] = XmlRpc::Variable(value); } \
  bool is_##name##_nil() const { return data[#name].is_nil(); }

namespace XmlRpc
{
  class Struct
  {
  protected:
    Variable data;
  public:
    operator Variable() const { return data; }

    Struct() {}
    Struct(const Variable& variable) : data(variable) {}
  };
}

#endif
