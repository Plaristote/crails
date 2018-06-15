#ifndef  DB_DATABASE_SETTINGS_HPP
# define DB_DATABASE_SETTINGS_HPP

# include <crails/databases.hpp>

namespace ODB
{
  Crails::Databases::DatabaseSettings get_database_settings_for(const std::string& name);
}

#endif
