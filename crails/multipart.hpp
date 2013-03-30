#ifndef  MULTIPART_HPP
# define MULTIPART_HPP

#include <Boots/Sync/semaphore.hpp>
#include <crails/cookie_data.hpp>

class Params;

struct MultipartParser
{
  MultipartParser(void) : sem(1), end(0)
  {
  }
  
  void         Parse(Params&);

  std::string  read_buffer;
  unsigned int to_read;
  unsigned int total_read;
  std::string  boundary;

  Sync::Semaphore sem;
  Sync::Semaphore end;

  // Context-linked attributes
  short           parsed_state;
  std::ofstream   file;
  std::string     mimetype;
  CookieData      content_data;
  std::string     content_disposition;
};

#endif