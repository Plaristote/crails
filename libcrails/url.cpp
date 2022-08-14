#include "url.hpp"
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <regex>

using namespace std;
using namespace Crails;

Url Url::from_string(const std::string& url)
{
  static const std::regex url_matcher("^(https?)://([^/:]+)(:([0-9]{1,5}))?/?(.*)$");
  std::smatch matches;

  if (std::regex_match(url, matches, url_matcher))
  {
    bool ssl = matches[1].str() == "https";
    return Url{
      /* ssl    */ ssl,
      /* host   */ matches[2].str(),
      /* port   */ matches[4].str().length() > 0 ? boost::lexical_cast<unsigned short>(matches[4].str()) : static_cast<unsigned short>(ssl ? 443 : 80),
      /* target */ matches[5].str()
    };
  }
  return Url{};
}

std::string Url::to_string() const
{
  std::stringstream stream;

  stream << "http";
  if (ssl) stream << 's';
  stream << "://" << host << ':' << port << '/' << target;
  return stream.str();
}
