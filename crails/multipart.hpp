#ifndef  MULTIPART_HPP
# define MULTIPART_HPP

# include <crails/cookie_data.hpp>
# include <fstream>

namespace Crails
{
  class Params;

  struct MultipartParser
  {
    void         initialize(Params&);
    void         parse(Params&);

    std::string  read_buffer;
    unsigned int to_read;
    unsigned int total_read;
    std::string  boundary;

    // Context-linked attributes
    short           parsed_state;
    std::ofstream   file;
    std::string     mimetype;
    DataTree        content_data;
    std::string     content_disposition;
  };
}

#endif
