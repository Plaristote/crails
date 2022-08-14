#ifndef  XMLRPC_FAULT_HPP
# define XMLRPC_FAULT_HPP

# include "variable.hpp"
# include <exception>

namespace XmlRpc
{
  class Fault : public std::exception
  {
  public:
    Fault(const XmlRpc::Variable& variable) : variable(variable)
    {}

    const char* what() const noexcept
    {
      return variable["faultString"].value.c_str();
    }

    int fault_code() const
    {
      return variable["faultCode"].as_int();
    }

  private:
    XmlRpc::Variable variable;
  };
}

#endif
