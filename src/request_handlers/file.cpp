#include "crails/request_handlers/file.hpp"
#include "crails/server.hpp"
#include "crails/params.hpp"
#include "crails/logger.hpp"
#include <boost/filesystem.hpp>

using namespace std;
using namespace Crails;

bool FileRequestHandler::operator()(const HttpServer::request& request, BuildingResponse& response, Params& params)
{
  if (request.method == "GET")
  {
    std::string fullpath       = params["uri"].as<string>();
    size_t      pos_get_params = fullpath.find('?');

    if (pos_get_params != std::string::npos)
      fullpath.erase(pos_get_params);
    fullpath = "public" + fullpath;

    if (boost::filesystem::is_directory(fullpath))
      fullpath += "/index.html";
    if (send_file(fullpath, response, Server::HttpCodes::ok))
    {
      params["response-data"]["code"] = (int)Server::HttpCodes::ok;
      return true;
    }
  }
  return false;
}

bool FileRequestHandler::send_file(const std::string& fullpath, BuildingResponse& response, Server::HttpCode code, unsigned int first_bit)
{
  file_cache.Lock();
  {
    bool               cached = cache_enabled && file_cache.Contains(fullpath);
    const std::string& str    = *(file_cache.Require(fullpath));

    if (&str != 0)
    {
      std::stringstream str_length;

      str_length << (str.size() - first_bit);
      response.set_headers("Content-Length", str_length.str());
      response.set_headers("Content-Type",   get_mimetype(strrchr(fullpath.c_str(), '.')));
      response.set_status_code(code);
      response.set_body(str.c_str() + first_bit, str.size() - first_bit);
      logger << Logger::Info << "# Delivering asset `" << fullpath << "` ";
      if (cache_enabled)
        logger << (cached ? "(was cached)" : "(was not cached)") << Logger::endl;
      else
      {
        file_cache.GarbageCollect();
        logger << "(cache disabled)" << Logger::endl;
      }
      file_cache.Unlock();
      return (true);
    }
  }
  file_cache.Unlock();
  return (false);
}
