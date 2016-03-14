#ifndef  ODB_DATABASE_LOADERS_HPP
# define ODB_DATABASE_LOADERS_HPP

# include "database.hpp"

namespace ODB
{
  template<typename V>
  static V defaults_to(const std::map<std::string, boost::any>& a, const std::string& k, const V def)
  {
    typename std::map<std::string, boost::any>::const_iterator it = a.find(k);

    if (it == a.end())
      return def;
    return boost::any_cast<V>(it->second);
  }

  inline odb::database* initialize_for_mysql(const Crails::Databases::DatabaseSettings& settings)
  {
#ifdef ODB_WITH_MYSQL
    return new odb::mysql::database(
      boost::any_cast<cosnt char*>(settings.at("user")),
      boost::any_cast<const char*>(settings.at("password")),
      boost::any_cast<const char*>(settings.at("name")),
      defaults_to<const char*> (settings, "host", ""),
      defaults_to<unsigned int>(settings, "port", 0)
      0,
      defaults_to<const char*>(settings, "charset", "")
    );
#else
    throw boost_ext::runtime_error("ODB was compiled without support for `mysql`");
    return nullptr;
#endif
  }

  inline odb::database* initialize_for_postgresql(const Crails::Databases::DatabaseSettings& settings)
  {
#ifdef ODB_WITH_PGSQL
    return new odb::pgsql::database(
      boost::any_cast<const char*>(settings.at("user")),
      boost::any_cast<const char*>(settings.at("password")),
      boost::any_cast<const char*>(settings.at("name")),
      defaults_to<const char*> (settings, "host",  ""),
      defaults_to<unsigned int>(settings, "port",  0),
      defaults_to<const char*> (settings, "extra", "")
    );
#else
    throw boost_ext::runtime_error("ODB was compiled without support for `pgsql`");
    return nullptr;
#endif
  }

  inline odb::database* initialize_for_sqlite(const Crails::Databases::DatabaseSettings& settings)
  {
#ifdef ODB_WITH_SQLITE
    return new odb::sqlite::database(
      boost::any_cast<const char*>(settings.at("name")),
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
    );
#else
    throw boost_ext::runtime_error("ODB was compiled without support for `sqlite`");
    return nullptr;
#endif
  }

  inline odb::database* initialize_for_oracle(const Crails::Databases::DatabaseSettings& settings)
  {
#ifdef ODB_WITH_ORACLE
    return new odb::oracle::database(
      boost::any_cast<const char*>(settings.at("user")),
      boost::any_cast<const char*>(settings.at("password")),
      boost::any_cast<const char*>(settings.at("name")),
      defaults_to<const char*> (settings, "host", ""),
      defaults_to<unsigned int>(settings, "port", 0)
    );
#else
    throw boost_ext::runtime_error("ODB was compiled without support for `oracle`");
    return nullptr;
#endif
  }
}

#endif
