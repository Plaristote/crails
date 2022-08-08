#include "crails/request_handlers/file.hpp"
#include "crails/server.hpp"
#include "crails/request.hpp"
#include "crails/params.hpp"
#include "crails/logger.hpp"
#include "crails/helpers.hpp"
#include "crails/utils/string.hpp"
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <time.h>

using namespace std;
using namespace Crails;
using namespace boost::beast::http;

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

static std::pair<unsigned int, unsigned int> range_from_header(const std::string& header)
{
  auto parts = split(header, '=');
  pair<unsigned int, unsigned int> range{0,0};

  if (parts.size() == 2)
  {
    parts = split(*parts.rbegin(), '-');
    range.first  = boost::lexical_cast<unsigned int>(*parts.begin());
    range.second = boost::lexical_cast<unsigned int>(*parts.rbegin());
  }
  return range;
}

void FileRequestHandler::operator()(Request& request, function<void(bool)> callback)
{
  if (request.connection->get_request().method() == boost::beast::http::verb::get)
  {
    boost::system::error_code ec;
    string                    fullpath;
    BuildingResponse&         response   = request.out;
    string                    dirty_path = request.params["uri"].as<string>();
    size_t                    pos        = dirty_path.find('?');
    Range                     range{0,0};

    if (pos != std::string::npos)
      dirty_path.erase(pos);
    boost::filesystem::canonical(Server::public_path + dirty_path, ec);
    if (ec == boost::system::errc::success)
      fullpath = boost::filesystem::absolute(Server::public_path + dirty_path).string();
    if (ec != boost::system::errc::success)
      response.set_status_code(HttpStatus::not_found);
    else if (fullpath.find(Server::public_path) == string::npos)
      response.set_status_code(HttpStatus::bad_request);
    else
    {
      if (boost::filesystem::is_directory(fullpath))
        fullpath += "/index.html";
      serve_compressed_file_if_possible(fullpath, response, request.params);
      if (request.params["headers"]["Range"].exists())
        range = range_from_header(request.params["headers"]["Range"].as<string>());
      if (request.params["headers"]["If-Modified-Since"].exists() &&
          if_not_modified(request.params, response, fullpath))
      {
        response.set_status_code(HttpStatus::not_modified);
        callback(true);
        return ;
      }
      else if (send_file(fullpath, response, HttpStatus::ok, range))
      {
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
    response.set_status_code(boost::beast::http::status::not_modified);
    return true;
  }
  return false;
}

bool FileRequestHandler::send_file(const std::string& fullpath, BuildingResponse& response, HttpStatus code, std::pair<unsigned int, unsigned int> range)
{
  file_cache.lock();
  {
    bool cached = cache_enabled && file_cache.contains(fullpath);
    auto file   = file_cache.require(fullpath);

    if (file)
    {
      const string& str = *file;
      std::stringstream str_length;

      if (range.second == 0)
        range.second = str.size();
      str_length << (range.second - range.first);
      response.set_headers("Content-Length", str_length.str());
      response.set_headers("Content-Type",   get_mimetype(fullpath));
      set_headers_for_file(response, fullpath);
      response.set_status_code(code);
      response.set_body(str.c_str() + range.first, range.second - range.first);
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
