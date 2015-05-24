#ifndef  SHELL_HPP
# define SHELL_HPP

# include <string>

class ShellString : public std::string
{
public:
  ShellString(void) {}
  ShellString(const std::string& str) : std::string(str) {}
  ShellString(const char*        str) : std::string(str) {}

  void          Colorize(const std::string& color);
  void          Backgroundize(const std::string& color);
  void          Blinking(void)   { SetEffect(5); }
  void          Underlined(void) { SetEffect(4); }
  void          Bolded(void)     { SetEffect(1); }
private:
  void          SetEffect(unsigned char code);
  unsigned char GetCodeFromColor(const std::string& color) const;
};

#endif

