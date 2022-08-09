#ifndef  FAYE_CLIENT_HPP
# define FAYE_CLIENT_HPP

# include <string>
# include <crails/datatree.hpp>
# include <crails/client.hpp>

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
    Settings settings;
    Crails::Client http;
  };
}

#endif
