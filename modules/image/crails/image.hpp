#ifndef  CRAILS_IMAGE_HPP
# define CRAILS_IMAGE_HPP

# include <Magick++.h>
# include <crails/params.hpp>
# include <boost/filesystem.hpp>

namespace Crails
{
  class BasicImage : public std::string
  {
  public:
    BasicImage() : original_image_loaded(false), format("png")
    {
    }

    const std::string& operator=(const std::string& name)
    {
      return std::string::operator=(name);
    }

    void operator=(const Crails::Params::File* file)
    {
      if (file)
        boost::filesystem::rename(file->temporary_path, get_filepath());
      else
        std::string::operator=("");
    }

    virtual void generate_thumbnails()
    {
    }

    virtual std::string get_default_image_path() const
    {
      return BasicImage::default_image_path;
    }

    std::string get_path(const std::string& transformation_name = "") const;

    void perform_crop(const std::string& transformation_name, unsigned int max_x, unsigned int max_y);
  protected:
    std::string get_filepath(const std::string& transformation_name = "") const;
    std::string get_filename(const std::string& transformation_name = "") const;
    void        generate_filename();

    void load_original_image();

    std::string        temporary_path;
    static std::string store_path, default_image_path;
    Magick::Image      original_image;
    bool               original_image_loaded;
    std::string        format;
  };
}


#endif
