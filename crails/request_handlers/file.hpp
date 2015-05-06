#ifndef  FILE_REQUEST_HANDLER_HPP
# define FILE_REQUEST_HANDLER_HPP

# include "../server.hpp"

class FileRequestHandler : public RequestHandler
{
  friend class CrailsServer;
public:
  FileRequestHandler() : RequestHandler("file")
  {
    cache_enabled = true;
  }

  bool operator()(const Server::request& request, BuildingResponse& response, Params& params);
  
  void set_cache_enabled(bool enable) { cache_enabled = enable; }
  bool is_cache_enabled(void) const   { return cache_enabled;   }

private:
  bool SendFile(const std::string& path, BuildingResponse& response, CrailsServer::HttpCode code, unsigned int first_bit = 0);

  bool      cache_enabled;
  FileCache file_cache;
};

#endif
