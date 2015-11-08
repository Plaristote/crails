#ifndef  CRAILS_IMAGE_HPP
# define CRAILS_IMAGE_HPP

# include <crails/params.hpp>
# include <boost/filesystem.hpp>

namespace Crails
{
  class BasicImage : public std::string
  {
  public:
    enum Option
    {
      PreserveAspectRatio
    };

    BasicImage() : format("png")
    {
    }

    BasicImage(const std::string& name) : std::string(name), format("png")
    {
    }

    std::string get_path(const std::string& transformation_name = "") const;

    void use_uploaded_file(const Crails::Params::File* file);

    virtual void cleanup_files();
    virtual void generate_thumbnails() {}

    virtual std::string get_default_image_path() const
    {
      return BasicImage::default_image_path;
    }

    virtual unsigned short get_default_image_quality() const
    {
      return BasicImage::default_image_quality;
    }

  protected:
    void perform_resize(const std::string& transformation_name, unsigned int x, unsigned int y, int options = 0);
    void perform_crop(const std::string& transformation_name, unsigned int max_x, unsigned int max_y, int options = 0);

    std::string get_filepath(const std::string& transformation_name = "") const;
    std::string get_filename(const std::string& transformation_name = "") const;
    void        generate_filename();

    const static std::string    store_path, default_image_path;
    const static unsigned short default_image_quality;
    std::string                 format;

  private:
    void get_geometry_for_crop_preserving_aspect_ratio(unsigned int width, unsigned int height, unsigned int& max_x, unsigned int& max_y);
  };
}


#endif
