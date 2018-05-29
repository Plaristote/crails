#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

using namespace boost;

namespace Crails
{
  std::string get_ssl_password(std::size_t max_length, asio::ssl::context_base::password_purpose purpose)
  {
    return "your_ssl_password";
  }
}
