#ifndef  XMLRPC_CLIENT_HPP
# define XMLRPC_CLIENT_HPP

# include <boost/network/protocol/http/client.hpp>
# include <boost/lexical_cast.hpp>
# include "variable.hpp"
# include "xml_for_method_call.hpp"

namespace XmlRpc
{
  class Client
  {
  protected:
    const std::string endpoint;
    boost::network::http::client client;

  public:
    Client(const std::string& endpoint);

    virtual void set_request_headers(boost::network::http::client::request& request)
    {
    }

    template<typename ...Args>
    std::vector<Variable> call(const std::string& method_name, Args... args)
    {
      std::string body = xml_for_method_call(method_name, args...);
      boost::network::http::client::request request(endpoint);
      DataTree response_data;

      request << boost::network::header("Connection",   "close");
      request << boost::network::header("User-Agent",   "crails-xmlrpc/0.1");
      request << boost::network::header("Content-Type", "text/xml");
      request << boost::network::header("Content-Length", boost::lexical_cast<std::string>(body.length()));
      request << boost::network::body(body);
      auto response = client.post(request);
      int  status   = boost::network::http::status(response);

      if (status >= 400)
        throw std::runtime_error("XmlRpc::Client::call responded with status >= 400");
      std::cout << "response body: " << boost::network::http::body(response) << std::endl;
      response_data.from_xml(boost::network::http::body(response));
      if (is_faulty_response(response_data))
        raise_xmlrpc_fault(response_data);
      return get_response_params(response_data);
    }

    bool is_faulty_response(const DataTree& data) const
    {
      return data["methodResponse"]["fault"].exists();
    }

    void raise_xmlrpc_fault(const DataTree& data) const;

    std::vector<XmlRpc::Variable> get_response_params(const DataTree& data) const;
  };
}

#endif
