#include <crails/image.hpp>
#include <Magick++.h>

using namespace std;
using namespace Crails;

void BasicImage::use_uploaded_file(const Crails::Params::File* file)
{
  if (file)
  {
    cleanup_files();
    generate_filename();
    boost::filesystem::rename(file->temporary_path, get_filepath());
  }
  else
    std::string::operator=("");
}

void BasicImage::cleanup_files()
{
  boost::filesystem::remove(get_filepath());
}

void BasicImage::generate_filename()
{
  std::string       chars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
  std::stringstream stream;

  stream << std::time(0) << '_';
  for (short i = 0 ; i < 5 ; ++i)
    stream << chars[std::rand() % chars.length()];
  std::string::operator=(stream.str());
}

std::string BasicImage::get_path(const std::string& transformation_name) const
{
  if (length() == 0)
    return get_default_image_path();
  return '/' + store_path + '/' + get_filename(transformation_name);
}

std::string BasicImage::get_filepath(const std::string& transformation_name) const
{
  return "public/" + store_path + '/' + get_filename(transformation_name);
}

std::string BasicImage::get_filename(const std::string& transformation_name) const
{
  if (transformation_name.length() > 0)
    return (*this) + '-' + transformation_name + '.' + format;
  return (*this) + '.' + format;
}

void BasicImage::perform_crop(const std::string& transformation_name, unsigned int max_x, unsigned int max_y, int options)
{
  Magick::Image image(get_filepath());

  if (image.columns() > max_x || image.rows() > max_y)
  {
    if (options & PreserveAspectRatio)
      get_geometry_for_crop_preserving_aspect_ratio(image.columns(), image.rows(), max_x, max_y);
    image.zoom(Magick::Geometry(max_x, max_y));
  }
  image.magick(format);
  image.write(get_filepath(transformation_name));
}

void BasicImage::get_geometry_for_crop_preserving_aspect_ratio(unsigned int width, unsigned int height, unsigned int& max_x, unsigned int& max_y)
{
  float width_ratio, height_ratio, resize_ratio;
  float long_side_max  = std::max(max_x, max_y);
  float short_side_max = std::min(max_x, max_y);

  if (width > height)
  {
    width_ratio  = long_side_max / width;
    height_ratio = short_side_max / height;
  }
  else
  {
    width_ratio  = short_side_max / height;
    height_ratio = long_side_max / width;
  }
  resize_ratio = std::min(width_ratio, height_ratio);
  max_x        = height * resize_ratio;
  max_y        = width  * resize_ratio;
}

void BasicImage::perform_resize(const std::string& transformation_name, unsigned int width, unsigned int height, int options)
{
  Magick::Image    image(get_filepath());
  Magick::Geometry size(width, height);

  size.aspect(options & PreserveAspectRatio);
  image.resize(size);
  image.magick(format);
  image.write(get_filepath(transformation_name));
}