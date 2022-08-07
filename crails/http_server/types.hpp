#ifndef  CRAILS_HTTP_TYPES_HPP
# define CRAILS_HTTP_TYPES_HPP

# include <boost/beast/http.hpp>

namespace Crails
{
  typedef boost::beast::http::request<boost::beast::http::string_body> HttpRequest;
  typedef boost::beast::http::response<boost::beast::http::string_body> HttpResponse;
}

#endif
