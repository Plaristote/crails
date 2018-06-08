#ifndef  FAYE_CLIENT_HPP
# define FAYE_CLIENT_HPP

# include <string>
# include <crails/datatree.hpp>
# include <boost/network/protocol/http/client.hpp>

namespace Faye
{
  class Client
  {
  public:
    struct Settings
    {
      const std::string protocol, hostname;
      const unsigned short port;
      const std::function<void(Data)> hook;
    };

    Client(const Settings& settings);

    void publish(const std::string& channel, Data message);
  private:
    std::string url_for_channel(const std::string& channel) const;

    Settings settings;
    boost::network::http::client http;
  };
}

#endif
