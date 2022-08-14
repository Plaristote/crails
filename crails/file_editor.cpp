#include "file_editor.hpp"
#include "read_file.hpp"
#include "render_file.hpp"
#include <regex>
#include <iostream>

CrailsFileEditor::CrailsFileEditor(const std::string& path, const std::string& symbol) : path(path), symbol(symbol)
{
}

bool CrailsFileEditor::load_file()
{
  if (Crails::read_file(path, contents))
  {
    std::regex pattern("^\\s*//\\s*" + symbol + "\\s*$", std::regex_constants::multiline | std::regex_constants::icase);
    auto match = std::sregex_iterator(contents.begin(), contents.end(), pattern);

    if (match != std::sregex_iterator())
    {
      position = match->position() + match->length() + 1;
      return true;
    }
    else
      std::cout << "[FILE] Cannot find insert pattern `" << symbol << "` in file " << path << std::endl;
  }
  else
    std::cout << "[FILE] Cannot open file " << path << std::endl;
  return false;
}

void CrailsFileEditor::insert(const std::string& text)
{
  contents.insert(position, text);
}

void CrailsFileEditor::add_include(const std::string& path)
{
  contents.insert(0, "#include \"" + path + "\"\n");
}

void CrailsFileEditor::save_file()
{
  Crails::RenderFile render_target;

  render_target.open(path);
  render_target.set_body(contents.c_str(), contents.length());
  std::cout << "[EDITED] File `" << path << "`  was updated." << std::endl;
}
