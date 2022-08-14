#ifndef  CRAILS_RENDER_FILE_HPP
# define CRAILS_RENDER_FILE_HPP

# include "render_target.hpp"
# include <fstream>

namespace Crails
{
  class RenderFile : public RenderTarget
  {
  public:
    bool open(std::string_view path)
    {
      file.open(path.data());
      return file.is_open();
    }

    void set_body(const char* str, size_t size) override
    {
      file.write(str, size);
      file.close();
    }

  private:
    std::ofstream file;
  };
}

#endif
