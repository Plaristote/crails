#ifndef  MY_MIGRATION_HPP
# define MY_MIGRATION_HPP

# include <odb/schema-catalog.hxx>
# include <list>
# include <functional>

# ifndef ODB_COMPILER
#  include <crails/odb/database.hpp>
# else
namespace ODB { class Database; }
# endif

namespace Db
{
  typedef std::function<bool (ODB::Database&, odb::schema_version)> MigrateFunction;

  struct Migration
  {
    std::string                          name;
    odb::schema_version                  version;
    std::function<bool (ODB::Database&)> runner;
  };

  struct Migrations
  {
    Migrations();

    bool run_for_version(ODB::Database& db, odb::schema_version version) const;
    std::list<Migration> list;

    operator MigrateFunction() const
    {
      return std::bind(&Db::Migrations::run_for_version, this, std::placeholders::_1, std::placeholders::_2);
    }
  };
}

# define ADD_MIGRATION(name, version, body) \
  Db::Migration name = { #name, version, [](ODB::Database& database) -> bool body };

#endif
