#ifndef  ODB_MIGRATE_HPP
# define ODB_MIGRATE_HPP

# include <odb/database.hxx>
# include <map>

# ifdef ODB_WITH_MYSQL
#  include <odb/mysql/database.hxx>
# endif

# ifdef ODB_WITH_PGSQL
#  include <odb/pgsql/database.hxx>
# endif

# ifdef ODB_WITH_SQLITE
#  include <odb/sqlite/database.hxx>
# endif

# ifdef ODB_WITH_ORACLE
#  include <odb/oracle/database.hxx>
# endif

# include <odb/schema-catalog.hxx>
# include <crails/logger.hpp>

namespace ODB
{
  class Database;

  template<typename T>
  void database_drop(T& db)
  {
    odb::transaction t(db.begin());

    odb::schema_catalog::drop_schema(db);
    t.commit();
  }

  template<typename T>
  void database_migrate(ODB::Database& self, T& database, std::function<bool (ODB::Database&, odb::schema_version)> callback)
  {
    using namespace Crails;
    odb::schema_version v(database.schema_version());
    odb::schema_version cv(odb::schema_catalog::current_version(database));

    logger << Logger::Info;
    if (v == 0)
    {
      odb::transaction t(database.begin());
      logger << ":: No schema version found\n";
      logger << ":: Initializing database from scratch" << Logger::endl;
      odb::schema_catalog::create_schema(database);
      t.commit();
    }
    else if (v < cv)
    {
      logger << ":: Migration" << '\n';
      for (v = odb::schema_catalog::next_version(database, v) ;
           v <= cv ;
           v = odb::schema_catalog::next_version(database, v))
      {
        {
          odb::transaction t(database.begin());

          logger << ":: Running migration to version " << v << '\n';
          odb::schema_catalog::migrate_schema_pre(database, v);
          t.commit();
        }

        {
          odb::transaction t(database.begin());

          if (callback && !(callback(self, v)))
            throw boost_ext::runtime_error("database migration failed");
          t.commit();
        }

        {
          odb::transaction t(database.begin());

          odb::schema_catalog::migrate_schema_post(database, v);
          t.commit();
        }
      }
    }
    else
      logger << ":: Nothing to do" << Logger::endl;
  }
}

#endif
