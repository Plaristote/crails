#include "../crails/odb/database_settings.hpp"
#include <crails/environment.hpp>

using namespace Crails;

extern const std::string default_configuration_name = "odb";

Databases::DatabaseSettings Db::get_database_settings_for(const std::string& name)
{
  const auto& settings = Databases::settings
    .at(Crails::environment)
    .at(default_configuration_name);

  return {
    { "type",     settings.at("type") },
    { "host",     settings.at("host") },
    { "name",     name },
    { "user",     settings.at("user") },
    { "password", settings.at("password") },
    { "port",     settings.at("port") }
  };
}
