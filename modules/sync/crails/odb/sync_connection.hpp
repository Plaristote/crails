#ifndef  ODB_SYNC_CONNECTION_HPP
# define ODB_SYNC_CONNECTION_HPP

# include <crails/odb/connection.hpp>
# include <crails/sync/transaction.hpp>

namespace ODB
{
  namespace Sync
  {
    class Connection : public ODB::Connection
    {
    public:
      static thread_local safe_ptr<ODB::Sync::Connection> instance;

      Connection() : ODB::Connection()
      {
        instance = std::shared_ptr<ODB::Sync::Connection>(this, [](ODB::Sync::Connection*) {});
      }

      ~Connection()
      {
        instance.reset();
      }

      static safe_ptr<ODB::Sync::Connection> get()
      {
        return instance;
      }

      void commit()
      {
        ODB::Connection::commit();
        if (use_sync_transaction)
          sync_transaction.commit();
      }

      void rollback()
      {
        ODB::Connection::rollback();
        if (use_sync_transaction)
          sync_transaction.rollback();
      }

      template<typename MODEL>
      void save(MODEL& model)
      {
        ODB::Connection::save<MODEL>(model);
        if (use_sync_transaction)
          sync_transaction.save(model);
      }

      template<typename MODEL>
      void destroy(MODEL& model)
      {
        ODB::Connection::destroy<MODEL>(model);
        if (use_sync_transaction)
          sync_transaction.destroy(model);
      }

      bool use_sync_transaction = true;
      ::Sync::Transaction sync_transaction;
    };
  }
}

#endif
