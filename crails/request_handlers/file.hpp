#ifndef  FILE_REQUEST_HANDLER_HPP
# define FILE_REQUEST_HANDLER_HPP

# include "../request_handler.hpp"

namespace Crails
{
  class FileRequestHandler : public RequestHandler
  {
    typedef std::pair<unsigned int, unsigned int> Range;
  public:
    FileRequestHandler() : RequestHandler("file"), file_cache(Crails::Server::get_file_cache())
    {
#ifdef SERVER_DEBUG
      cache_enabled = false;
#else
      cache_enabled = true;
#endif
    }

    void operator()(Request& params, std::function<void(bool)> callback) const override;
    bool send_file(const std::string& path, BuildingResponse& response, HttpStatus code, Range range = {0,0}) const;

    void set_cache_enabled(bool enable) { cache_enabled = enable; }
    bool is_cache_enabled(void) const   { return cache_enabled;   }

  protected:
    virtual void set_headers_for_file(BuildingResponse& response, const std::string& fullpath) const {}

  private:
    bool if_not_modified(Params&, BuildingResponse&, const std::string& path) const;

    bool       cache_enabled;
    FileCache& file_cache;
  };
}
  
#endif
