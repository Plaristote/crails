#include "client.hpp"
#include "fault.hpp"
#include "server.hpp"

using namespace std;
using namespace XmlRpc;

Client::Client(const string& url) :
  endpoint(Crails::Url::from_string(url)),
  client(endpoint.host, endpoint.port)
{
}

void Client::raise_xmlrpc_fault(const DataTree& data) const
{
  XmlRpc::Variable fault_struct = XmlRpc::Variable::from_data(data["methodResponse"]["fault"]["value"]);

  std::cout << "Fault struct: " << fault_struct.name << std::endl;
  throw Fault(fault_struct);
}

XmlRpc::Variable Client::get_response_variable(const DataTree& data) const
{
  const Data param = data["methodResponse"]["params"]["param"];

  return Variable::from_data(param["value"]);
}
