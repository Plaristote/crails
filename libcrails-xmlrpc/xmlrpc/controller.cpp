#include "controller.hpp"

using namespace XmlRpc;
using namespace std;

void Controller::endpoint()
{
  string method_name = params["document"]["methodCall"]["methodName"];

  for (const auto& method : methods)
  {
    if (method->name == method_name)
    {
      method->call(params);
      return ;
    }
  }
  raise_unknown_method();
}

void Controller::raise_unknown_method()
{
  respond_with_fault(634, "Invalid method name");
}

void Controller::respond_with_fault(int code, const std::string& message)
{
  stringstream stream;

  stream << "<methodResponse>"
         <<   "<fault>"
         <<     "<value>"
         <<       "<struct>"
         <<         "<member>"
         <<           "<name>faultCode</name>"
         <<           "<value><int>" << code << "</int></value>"
         <<         "</member>"
         <<         "<member>"
         <<           "<name>faultString</name>"
         <<           "<value><string>" + message + "</string></value>"
         <<         "</member>"
         <<       "</struct>"
         <<     "</value>"
         <<   "</fault>"
         << "</methodResponse>";
  Crails::Controller::render(XML, stream.str());
}

void Controller::respond_with(const XmlRpc::Variable& variable)
{
  stringstream stream;

  stream << "<methodResponse>"
         <<   "<params>"
         <<     "<param>"
         <<       "<value>";
  variable.to_xml(stream);
  stream <<       "</value>"
         <<     "</param>"
         <<   "</params>"
         << "</methodResponse>";
  Crails::Controller::render(XML, stream.str());
}
