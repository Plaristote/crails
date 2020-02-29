#include "crails/request_handlers/file.hpp"
#include "crails/server.hpp"
#include "crails/params.hpp"
#include "crails/logger.hpp"
#include <boost/filesystem.hpp>
#include <time.h>

using namespace std;
using namespace Crails;

const vector<pair<string, string> > compression_strategies = {{"br", "br"}, {"gzip", "gz"}};

static bool accepts_encoding(Params& params, const std::string& name)
{
  const string accepts = params["headers"]["Accept-Encoding"].as<string>();

  return accepts.find(name) != std::string::npos;
}

static void serve_compressed_file_if_possible(string& fullpath, BuildingResponse& response, Params& params)
{
  for (const auto& strategy : compression_strategies)
  {
    if (accepts_encoding(params, strategy.first) && boost::filesystem::exists(fullpath + '.' + strategy.second))
    {
      response.set_headers("Content-Encoding", strategy.first);
      fullpath += '.' + strategy.second;
      break ;
    }
  }
}

void FileRequestHandler::operator()(const HttpServer::request& request, BuildingResponse& response, Params& params, function<void(bool)> callback)
{
  if (request.method == "GET")
  {
    boost::system::error_code ec;
    string                    fullpath;
    string                    dirty_path = params["uri"].as<string>();
    size_t                    pos        = dirty_path.find('?');

    if (pos != std::string::npos)
      dirty_path.erase(pos);
    boost::filesystem::canonical(Server::public_path + dirty_path, ec);
    if (ec == boost::system::errc::success)
      fullpath = boost::filesystem::absolute(Server::public_path + dirty_path).string();
    if (ec != boost::system::errc::success)
      params["response-data"]["code"] = (int)Server::HttpCodes::not_found;
    else if (fullpath.find(Server::public_path) == string::npos)
      params["response-data"]["code"] = (int)Server::HttpCodes::bad_request;
    else
    {
      if (boost::filesystem::is_directory(fullpath))
        fullpath += "/index.html";
      serve_compressed_file_if_possible(fullpath, response, params);
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
  file_cache.lock();
  {
    bool cached = cache_enabled && file_cache.contains(fullpath);
    auto file   = file_cache.require(fullpath);

    if (file)
    {
      const string& str = *file;
      std::stringstream str_length;

      str_length << (str.size() - first_bit);
      response.set_headers("Content-Length", str_length.str());
      response.set_headers("Content-Type",   get_mimetype(fullpath));
      set_headers_for_file(response, fullpath);
      response.set_status_code(code);
      response.set_body(str.c_str() + first_bit, str.size() - first_bit);
      logger << Logger::Info << "# Delivering asset `" << fullpath << "` ";
      if (cache_enabled)
        logger << (cached ? "(was cached)" : "(was not cached)") << Logger::endl;
      else
      {
        file_cache.garbage_collect();
        logger << "(cache disabled)" << Logger::endl;
      }
      file_cache.unlock();
      return true;
    }
  }
  file_cache.unlock();
  return false;
}
