#ifndef  DATABASE_URL_HPP
# define DATABASE_URL_HPP

# include <boost/lexical_cast.hpp>
# include <crails/logger.hpp>

namespace Crails
{
  struct DatabaseUrl
  {
    DatabaseUrl(const char* str)
    {
      if (str)
        initialize(str);
    }

    void initialize(const std::string& url)
    {
      if (url.size() > 0)
      {
        try
        {
          type     = substr(url, get_protocol_range(url));
          hostname = substr(url, get_hostname_range(url));
          username = substr(url, get_username_range(url));
          password = substr(url, get_password_range(url));
          port     = boost::lexical_cast<unsigned short>(substr(url, get_port_range(url)));
          database_name = substr(url, get_database_name_range(url));
        }
        catch (std::exception& e)
        {
          logger << "Failed to read database url string " << url << Logger::endl;
          throw e;
        }
      }
    }

    std::string    type, hostname, username, password, database_name;
    unsigned int   port;

  private:
    std::string substr(const std::string& url, std::pair<int,int> range)
    {
      return url.substr(range.first, range.second - range.first);
    }

    std::pair<int, int> get_protocol_range(const std::string& url)
    {
      return { 0, url.find("://") };
    }

    std::pair<int, int> get_username_range(const std::string& url)
    {
      int start_name = get_protocol_range(url).second + 3;
      return { start_name, url.find(":", start_name) };
    }

    std::pair<int, int> get_password_range(const std::string& url)
    {
      int start_password = get_username_range(url).second + 1;
      return { start_password, url.find("@", start_password) };
    }

    std::pair<int, int> get_hostname_range(const std::string& url)
    {
      int start_hostname = get_password_range(url).second + 1;
      return { start_hostname, url.find(":", start_hostname) };
    }

    std::pair<int, int> get_port_range(const std::string& url)
    {
      int start_port = get_hostname_range(url).second + 1;
      return { start_port, url.find("/", start_port) };
    }

    std::pair<int, int> get_database_name_range(const std::string& url)
    {
      int start_database_name = get_port_range(url).second + 1;
      return { start_database_name, url.length() };
    }
  };
}

#endif
