#ifndef  CRAILS_ATTACHMENT_HPP
# define CRAILS_ATTACHMENT_HPP

# include "params.hpp"

namespace Crails
{
  class Attachment : public std::string
  {
  public:
    Attachment()
    {
    }

    Attachment(const std::string& uid);

    const std::string& get_extension() const { return extension; }
    const std::string& get_mimetype() const { return mimetype; }
    std::string get_path() const;

    void use_uploaded_file(const Crails::Params::File* file);

    virtual void cleanup_files();

    virtual std::string get_default_store_path() const
    {
      return Attachment::default_store_path;
    }

  protected:
    std::string get_filepath() const;
    std::string get_filename() const;
    void        generate_uid();

    const static std::string default_store_path;
    std::string              extension, mimetype;
  };
}

#endif
