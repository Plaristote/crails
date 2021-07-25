#include <crails/attachment.hpp>
#include <crails/utils/string.hpp>

using namespace std;
using namespace Crails;

Attachment::Attachment(const string& uid) : std::string(uid)
{
  list<string> parts = split(uid, '.');

  if (parts.size() > 3)
  {
    auto part = parts.rbegin();

    extension = *part++;
    mimetype  = *part;
  }
}

static string find_extension_in_filename(const string& value)
{
  list<string> parts = split(value, '.');

  if (parts.size() > 1)
    return *parts.rbegin();
  return ".blob";
}

void Attachment::use_uploaded_file(const Crails::Params::File* file)
{
  if (file)
  {
    cleanup_files();
    extension = find_extension_in_filename(file->name);
    mimetype = file->mimetype;
    generate_uid();
    boost::filesystem::copy_file(file->temporary_path, get_filepath());
  }
  else
    std::string::operator=("");
}

void Attachment::cleanup_files()
{
  boost::filesystem::remove(get_filepath());
}

void Attachment::generate_uid()
{
  std::string       charset("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
  std::stringstream stream;

  stream << std::time(0) << '_';
  stream << generate_random_string(charset, 5);
  std::string::operator=(stream.str() + '.' + mimetype + '.' + extension);
}

std::string Attachment::get_path() const
{
  if (length() == 0)
    return get_default_store_path();
  return '/' + get_default_store_path() + '/' + get_filename();
}

std::string Attachment::get_filepath() const
{
  return boost::filesystem::initial_path().string() + "/public/" + get_default_store_path() + '/' + get_filename();
}

std::string Attachment::get_filename() const
{
  return (*this) + '.' + extension;
}
