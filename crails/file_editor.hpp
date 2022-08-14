#pragma once
#include <string>

class CrailsFileEditor
{
  std::string  path, symbol;
  std::string  contents;
  unsigned int position;
public:
  CrailsFileEditor(const std::string& path, const std::string& symbol);

  bool load_file();
  void insert(const std::string& text);
  void add_include(const std::string& path);
  void save_file();
};
