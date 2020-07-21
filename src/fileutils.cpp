#include <regex>
#include <string>

using namespace std;

struct ExtensionMatch
{
  ExtensionMatch(const std::string& extension, const std::string& mime) :
    pattern(extension),
    regexp(extension + "$", std::regex_constants::ECMAScript | std::regex_constants::icase),
    mime(mime)
  {
  }

  std::string pattern;
  regex       regexp;
  std::string mime;
};

std::string get_mimetype(const std::string& filename)
{
  static const ExtensionMatch extensions[] = {
    ExtensionMatch("(htm|html)(.gz|.br)?", "text/html"),
    ExtensionMatch("js(.gz|.br)?",         "text/javascript"),
    ExtensionMatch("css(.gz|.br)?",        "text/css"),
    ExtensionMatch("png",                  "image/png"),
    ExtensionMatch("(jpg|jpeg)",           "image/jpg"),
    ExtensionMatch("bmp",                  "image/bmp"),
    ExtensionMatch("svg",                  "image/svg+xml"),
    ExtensionMatch("txt",                  "text/txt")
  };

  for (unsigned short i = 0 ; i < 8 ; ++i)
  {
    if (regex_search(filename, extensions[i].regexp))
      return (extensions[i].mime);
  }
  return ("application/octet-stream");
}
