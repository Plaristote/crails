#include "crails/request_handlers/file.hpp"
#include "crails/server.hpp"
#include "crails/params.hpp"
#include "crails/logger.hpp"
#include <boost/filesystem.hpp>
#include <time.h>

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
      if (params["headers"]["If-Modified-Since"].exists() &&
          if_not_modified(params, response, fullpath))
      {
        params["response-data"]["code"] = (int)Server::HttpCodes::not_modified;
        callback(true);
	return ;
      }
      else if (send_file(fullpath, response, Server::HttpCodes::ok))
      {
        params["response-data"]["code"] = (int)Server::HttpCodes::ok;
        callback(true);
        return ;
      }
    }
  }
  callback(false);
}

static std::time_t http_date_to_timestamp(const std::string& str)
{
  static const char format[] = "%a, %d %b %Y %H:%M:%S %Z"; // rfc 1123
  struct tm tm;
  bzero(&tm, sizeof(tm));
  if (strptime(str.c_str(), format, &tm))
    return mktime(&tm);
  return 0;
}

bool FileRequestHandler::if_not_modified(Params& params, BuildingResponse& response, const string& fullpath)
{
  const string str_time = params["headers"]["If-Modified-Since"].as<string>();
  time_t condition_time = http_date_to_timestamp(str_time);
  time_t modified_time  = boost::filesystem::last_write_time(fullpath);

  if (modified_time <= condition_time)
  {
    response.set_status_code(Server::HttpCodes::not_modified);
    return true;
  }
  return false;
}

bool FileRequestHandler::send_file(const std::string& fullpath, BuildingResponse& response, Server::HttpCode code, unsigned int first_bit)
{
  file_cache.Lock();
  {
    bool               cached = cache_enabled && file_cache.Contains(fullpath);
    const std::string& str    = *(file_cache.Require(fullpath));
    const std::string* ptr    = &str;

    if (ptr != 0)
    {
      std::stringstream str_length;

      str_length << (str.size() - first_bit);
      response.set_headers("Content-Length", str_length.str());
      response.set_headers("Content-Type",   get_mimetype(strrchr(fullpath.c_str(), '.')));
      set_headers_for_file(response, fullpath);
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
