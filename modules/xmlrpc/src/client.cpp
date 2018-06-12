#include "../crails/xmlrpc/client.hpp"
#include "../crails/xmlrpc/fault.hpp"
#include <crails/server.hpp>

using namespace std;
using namespace XmlRpc;

typedef boost::network::http::client_options<boost::network::http::client::tag_type>
  client_options;

Client::Client(const string& endpoint) : endpoint(endpoint), client(client_options().io_service(Crails::Server::get_io_service()))
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
