#ifdef ASYNC_SERVER
# include <boost/asio.hpp>
# include <boost/asio/ssl.hpp>
# include <boost/shared_ptr.hpp>

namespace Crails {
  std::string get_ssl_password(std::size_t max_length, boost::asio::ssl::context_base::password_purpose purpose)
  {
    return "your_ssl_password";
  }
}
#endif
