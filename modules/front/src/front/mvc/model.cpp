#include <crails/front/mvc/model.hpp>
#include <crails/front/http.hpp>

using namespace Crails::Front;
using namespace client;
using namespace std;

Crails::Front::Promise Model::fetch()
{
  auto request = Http::Request::get(get_url());

  request->set_headers({{"Accept", get_content_type()}});
  return request->send().then([this, request]()
  {
    auto response = request->get_response();

    if (response->ok())
    {
      if (response->has_body())
        parse(response->get_response_text());
      synced.trigger();
    }
  });
}

Crails::Front::Promise Model::save()
{
  auto request = Http::Request::make(get_id() == 0 ? "post" : "put", get_url());
  
  request->set_headers({
    {"Content-Type", get_content_type()},
    {"Accept",       get_content_type()}
  });
  request->set_body(get_payload());
  return request->send().then([this, request]()
  {
    auto response = request->get_response();
    
    if (response->ok())
    {
      if (response->has_body())
        parse(response->get_response_text());
      synced.trigger();
    }
  });
}

std::string ArchiveModel::get_payload()
{
  OArchive archive;

  serialize(archive);
  return archive.as_string();
}

void ArchiveModel::parse(const std::string& str)
{
  IArchive archive;

  archive.set_data(str);
  serialize(archive);
}

std::string ArchiveModel::get_content_type() const
{
  return Archive::mimetype;
}

std::string JsonModel::get_payload()
{
  if (get_resource_name().length() > 0)
  {
    std::stringstream stream;
    stream << '{' << '"' << get_resource_name() << '"' << ':' << to_json() << '}';

    return stream.str();
  }
  return to_json();
}

void JsonModel::parse(const std::string& str)
{
  DataTree data;

  data.from_json(str);
  if (get_resource_name().length() > 0 && data[get_resource_name()].exists())
    from_json(data[get_resource_name()]);
  else
    from_json(data);
}

std::string JsonModel::get_content_type() const
{
  return "application/json";
}
