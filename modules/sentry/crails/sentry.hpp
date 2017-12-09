#ifndef  SENTRY_HPP
# define SENTRY_HPP

# include <crails/datatree.hpp>
# include <crails/utils/backtrace.hpp>
# include <boost/network/protocol/http/client.hpp>

class Sentry
{
  static const std::string sentry_key,
                           sentry_secret,
                           project_id,
                           server_url,
                           server_protocol;
public:
  Sentry();

  void capture_exception(Data, const std::exception&);

private:
  void set_message_context(Data);
  void set_message_request(Data message, Data params);
  void initialize_exception_message(Data message, Data params, const std::exception&);
  void initialize_backtrace(Data, const boost_ext::backtrace&);

  void send_message(Data);
  static std::string get_server_url();

  boost::network::http::client http;
};

#endif
