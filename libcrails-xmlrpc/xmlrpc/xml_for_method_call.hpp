#ifndef  XMLRPC_XML_FOR_METHOD_CALL_HPP
# define XMLRPC_XML_FOR_METHOD_CALL_HPP

# include "variable.hpp"
# include <sstream>

namespace XmlRpc
{
  template<typename ...Args>
  std::string xml_for_method_call(const std::string& method_name, Args... args)
  {
    std::stringstream stream;
    std::vector<XmlRpc::Variable> array = { args... };

    stream << "<?xml version=\"1.0\"?>\n";
    stream << "<methodCall>\n"
           << "  <methodName>" << method_name << "</methodName>\n"
           << "  <params>\n";
    for (const auto& variable : array)
    {
      stream << "    <param>\n"
             << "      <value>\n";
      variable.to_xml(stream);
      stream << '\n';
      stream << "      </value>\n"
             << "    </param>\n";
    }
    stream << "  </params>\n";
    stream << "</methodCall>";
    return stream.str();
  }
}

#endif
