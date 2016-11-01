#include <regex>
#include <string>

using namespace std;

struct ExtensionMatch
{
  ExtensionMatch(std::string extension, std::string mime) : pattern(extension), mime(mime)
  {
    regexp = std::regex(extension + "$", std::regex_constants::ECMAScript | std::regex_constants::icase);
  }

  std::string pattern;
  regex       regexp;
  std::string mime;
};

std::string get_mimetype(const std::string& filename)
{
  static const ExtensionMatch extensions[] = {
    ExtensionMatch("(htm|html)", "text/html"),
    ExtensionMatch("js",         "text/javascript"),
    ExtensionMatch("css",        "text/css"),
    ExtensionMatch("png",        "image/png"),
    ExtensionMatch("(jpg|jpeg)", "image/jpg"),
    ExtensionMatch("bmp",        "image/bmp"),
    ExtensionMatch("txt",        "text/txt")
  };

  for (unsigned short i = 0 ; i < 7 ; ++i)
  {
    if (regex_search(filename, extensions[i].regexp))
      return (extensions[i].mime);
  }
  return ("application/octet-stream");
}
