#ifndef  XMLRPC_VARIABLE_HPP
# define XMLRPC_VARIABLE_HPP

# include <map>
# include <list>
# include <string>
# include <crails/datatree.hpp>

namespace XmlRpc
{
  template<typename T>
  struct CastMethod;

  struct Variable
  {
    std::string name;
    std::string value;

    std::map<std::string, Variable> members;
    std::list<Variable>             array;

    static Variable from_data(const Data data_value);
    static Variable from_date(const std::time_t);

    Variable();
    Variable(int number);
    Variable(double number);
    Variable(bool boolean);
    Variable(const std::string& str);

    template<typename T>
    Variable(const std::vector<T>& items)
    {
      name = "array";
      for (auto item : items)
        array.push_back(Variable(item));
    }

    const Variable& operator[](const char* name) const;
    const Variable& operator[](const std::string& name) const;
    const Variable& operator[](unsigned int i) const;

    operator int()         const { return as_int();     }
    operator double()      const { return as_double();  }
    operator bool()        const { return as_boolean(); }
    operator std::string() const { return as_string();  }
    operator std::time_t() const { return as_date();    }

    bool is_nil() const;

    int as_int() const;
    double as_double() const;
    bool as_boolean() const;
    const std::string& as_string() const;
    std::time_t as_date() const;

#define VARIABLE_AS_ARRAY(method_ptr_type) \
    template<typename T> \
    std::vector<T> as_array(typename CastMethod<T>::method_ptr_type method_ptr) const \
    { \
      std::vector<T> result; \
\
      validate_type_or_raise("array"); \
      for (const auto& item : array) \
        result.push_back((item.*method_ptr)()); \
      return result; \
    } \

    VARIABLE_AS_ARRAY(Type)
    VARIABLE_AS_ARRAY(TypeRef)

    void to_xml(std::stringstream&) const;

  private:
    void validate_type_or_raise(const std::string& type) const;
    void from_struct(const Data data);
    void from_array(const Data data);
  };

  template<typename T>
  struct CastMethod
  {
    typedef T        (Variable::*Type)()    const;
    typedef const T& (Variable::*TypeRef)() const;
  };
}

#endif
