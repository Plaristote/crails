#ifndef  CRAILS_IMAGE_HPP
# define CRAILS_IMAGE_HPP

# include <crails/params.hpp>
# include <boost/filesystem.hpp>

namespace Crails
{
  class BasicImage : public std::string
  {
  public:
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

  protected:
    void perform_crop(const std::string& transformation_name, unsigned int max_x, unsigned int max_y);

    std::string get_filepath(const std::string& transformation_name = "") const;
    std::string get_filename(const std::string& transformation_name = "") const;
    void        generate_filename();

    static std::string store_path, default_image_path;
    std::string        format;
  };
}


#endif
