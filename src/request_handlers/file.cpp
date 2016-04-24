#include "crails/request_handlers/file.hpp"
#include "crails/server.hpp"
#include "crails/params.hpp"
#include "crails/logger.hpp"
#include <boost/filesystem.hpp>

using namespace std;
using namespace Crails;

static const string pwd         = boost::filesystem::current_path().string();
static const string public_path = boost::filesystem::canonical(pwd + "/public").string();

void FileRequestHandler::operator()(const HttpServer::request& request, BuildingResponse& response, Params& params, function<void(bool)> callback)
{
  if (request.method == "GET")
  {
    boost::system::error_code ec;
    string                    fullpath = params["uri"].as<string>();
    size_t                    pos      = fullpath.find('?');

    if (pos != std::string::npos)
      fullpath.erase(pos);
    fullpath = boost::filesystem::canonical(public_path + fullpath, ec).string();
    if (ec != boost::system::errc::success)
      params["response-data"]["code"] = (int)Server::HttpCodes::not_found;
    else if (fullpath.find(public_path) == string::npos)
      params["response-data"]["code"] = (int)Server::HttpCodes::bad_request;
    else
    {
      if (boost::filesystem::is_directory(fullpath))
        fullpath += "/index.html";
      if (send_file(fullpath, response, Server::HttpCodes::ok))
      {
        params["response-data"]["code"] = (int)Server::HttpCodes::ok;
        callback(true);
        return ;
      }
    }
  }
  callback(false);
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
