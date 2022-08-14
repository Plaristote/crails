#ifndef  XMLRPC_CLIENT_HPP
# define XMLRPC_CLIENT_HPP

# include <boost/lexical_cast.hpp>
# ifdef CRAILS_B2_INCLUDE
#  include "../../libcrails-http-client/client.hpp"
#  include "url.hpp"
# else
#  include "../client.hpp"
#  include "../url.hpp"
# endif
# include "variable.hpp"
# include "xml_for_method_call.hpp"

namespace XmlRpc
{
  class Client
  {
  protected:
    const Crails::Url endpoint;
    Crails::Client client;

  public:
    Client(const std::string& endpoint);

    template<typename ...Args>
    Variable call(const std::string& method_name, Args... args)
    {
      std::string          body = xml_for_method_call(method_name, args...);
      Crails::HttpRequest  request{boost::beast::http::verb::post, endpoint.target, 11};
      Crails::HttpResponse response;
      DataTree response_data;

      request.set(Crails::HttpHeader::connection,   "close");
      request.set(Crails::HttpHeader::user_agent,   "crails-xmlrpc/0.2");
      request.set(Crails::HttpHeader::content_type, "text/xml");
      request.content_length(body.length());
      request.body() = body;
      client.connect();
      response = client.query(request);
      if (static_cast<short>(response.result()) >= 400)
        throw std::runtime_error("XmlRpc::Client::call responded with status >= 400");
      response_data.from_xml(std::string(response.body()));
      if (is_faulty_response(response_data))
        raise_xmlrpc_fault(response_data);
      return get_response_variable(response_data);
    }

  private:
    bool is_faulty_response(const DataTree& data) const
    {
      return data["methodResponse"]["fault"].exists();
    }

    void raise_xmlrpc_fault(const DataTree& data) const;

    XmlRpc::Variable get_response_variable(const DataTree& data) const;
  };
}

#endif
