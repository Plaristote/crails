#include <Boots/Utils/shell.hpp>
#include <sstream>

using namespace std;

unsigned char ShellString::GetCodeFromColor(const string& color) const
{
  const char*    colors[] = { "black", "red", "green", "brown", "blue", "magenta", "cyan", "gray", 0 };
  unsigned char  codes[]  = { 30,      31,    32,      33,      34,     35,        36,     37,     0 };

  for (unsigned short i = 0 ; colors[i] != 0 ; ++i)
  {
    if (color == colors[i])
      return (codes[i]);
  }
  return (codes[0]);
}

void ShellString::SetEffect(unsigned char code)
{
  stringstream str;

  str << "\033[" << (unsigned short)code << 'm' << *this << "\033[0m";
  *this = str.str();
}

void ShellString::Colorize(const string& color)
{
#ifndef _WIN32 
  SetEffect(GetCodeFromColor(color));
#endif
}

void ShellString::Backgroundize(const string& color)
{
#ifndef _WIN32
  SetEffect(GetCodeFromColor(color) + 10);
#endif
}
