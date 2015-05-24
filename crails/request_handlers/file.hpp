#ifndef  FILE_REQUEST_HANDLER_HPP
# define FILE_REQUEST_HANDLER_HPP

# include "../server.hpp"

namespace Crails
{
  class FileRequestHandler : public RequestHandler
  {
    friend struct Server;
  public:
    FileRequestHandler() : RequestHandler("file")
    {
      cache_enabled = true;
    }

    bool operator()(const HttpServer::request& request, BuildingResponse& response, Params& params);

    void set_cache_enabled(bool enable) { cache_enabled = enable; }
    bool is_cache_enabled(void) const   { return cache_enabled;   }

  private:
    bool send_file(const std::string& path, BuildingResponse& response, Server::HttpCode code, unsigned int first_bit = 0);

    bool      cache_enabled;
    FileCache file_cache;
  };
}
  
#endif
