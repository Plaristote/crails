#include "crails/server.hpp"
#include "crails/params.hpp"

using namespace std;

#ifdef ASYNC_SERVER
void BodyParser::wait_for_body(const Server::request& request, CrailsServerTraits::Response response, Params& params)
{
  Sync::Semaphore    sem(0);
  unsigned int       to_read    = params["header"]["Content-Length"];
  unsigned int       total_read = 0;
  std::string        read_buffer;
  Regex              get_boundary;
  
  callback = [this, &sem, &to_read, &total_read, &read_buffer, &response](boost::iterator_range<char const*> range,
                        boost::system::error_code error_code,
                        size_t size_read,
                        Server::connection_ptr connection_ptr)
  {
    std::cout << "Reading buffer..." << std::endl;
    for (unsigned int i = 0 ; i < size_read ; ++i)
      read_buffer += range[i];
    total_read += size_read;
    if (total_read == to_read)
    {
      std::cout << "Finished reading..." << std::endl;
      sem.Post();
    }
    else
      response->read(callback);
  };
  response->read(callback);
  sem.Wait();
  body_received(request, response, params);
  params.response_parsed.Post();
}
#else
void BodyParser::wait_for_body(const Server::request& request, CrailsServerTraits::Response response, Params& params)
{
  body_received(request, response, params);
}
#endif