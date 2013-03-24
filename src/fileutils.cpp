#include <Boots/Utils/regex.hpp>
#include <string>

using namespace std;

struct ExtensionMatch
{
  ExtensionMatch(std::string extension, std::string mime) : pattern(extension), mime(mime)
  {
    regex.SetPattern(extension + "$", REG_ICASE | REG_EXTENDED);
  }

  std::string pattern;
  Regex       regex;
  std::string mime;
};

std::string GetMimeType(const std::string& filename)
{
  static const ExtensionMatch extensions[] = {
    ExtensionMatch("(htm|html)", "text/html"),
    ExtensionMatch("js",         "script/javascript"),
    ExtensionMatch("css",        "text/css"),
    ExtensionMatch("png",        "image/png"),
    ExtensionMatch("(jpg|jpeg)", "image/jpg"),
    ExtensionMatch("bmp",        "image/bmp"),
    ExtensionMatch("txt",        "text/txt")
  };

  for (unsigned short i = 0 ; i < 7 ; ++i)
  {
    //if (('.' + extensions[i].pattern) == filename)
    if (!(extensions[i].regex.Match(filename)))
      return (extensions[i].mime);
  }
  return ("application/octet-stream");
}