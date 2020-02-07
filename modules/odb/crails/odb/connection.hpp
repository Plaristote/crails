#ifndef  ODB_CONNECTION_HPP
# define ODB_CONNECTION_HPP

# include <crails/safe_ptr.hpp>
# include <odb/pgsql/query.hxx>
# include <odb/result.hxx>
# include <odb/database.hxx>
# include "transaction.hpp"
# include "exception.hpp"
# include "id_type.hpp"
# include <crails/utils/timer.hpp>

namespace ODB
{
  class Connection
  {
  public:
    static thread_local safe_ptr<ODB::Connection> instance;

    Connection();
    ~Connection();

    static safe_ptr<ODB::Connection> get() { return instance; }

    float time = 0.f;

    template<typename MODEL>
    void start_transaction_for()
    {
      transaction.require(MODEL().get_database_name());
    }

    template<typename MODEL>
    void start_transaction_for(const MODEL& model)
    {
      transaction.require(model.get_database_name());
    }

    void commit();
    void rollback();

    template<typename MODEL>
    unsigned long count(odb::query<MODEL> query = odb::query<MODEL>(true))
    {
      start_transaction_for<MODEL>();
      return transaction.get_database()
        .query_value<typename MODEL::Count>(query).value;
    }

    template<typename MODEL_PTR>
    bool find_one(MODEL_PTR& model, odb::query<typename MODEL_PTR::element_type> query = odb::query<typename MODEL_PTR::element_type>(true))
    {
      typedef typename MODEL_PTR::element_type MODEL;
      Utils::Timer timer;

      start_transaction_for<MODEL>();
      model = transaction.get_database().query_one<MODEL>(query);
      time += timer.GetElapsedSeconds();
      return model.get() != 0;
    }

    template<typename MODEL_PTR>
    bool find_one(MODEL_PTR& model, ODB::id_type id)
    {
      return find_one(model, odb::query<typename MODEL_PTR::element_type>::id == id);
    }

    template<typename MODEL>
    bool find(odb::result<MODEL>& results, odb::query<MODEL> query = odb::query<MODEL>(true))
    {
      Utils::Timer timer;

      start_transaction_for<MODEL>();
      results = transaction.get_database().query<MODEL>(query);
      time += timer.GetElapsedSeconds();
      return !results.empty();
    }

    template<typename MODEL>
    void save(MODEL& model)
    {
      Utils::Timer timer;

      start_transaction_for(model);
      model.before_save();
      model.save(transaction.get_database());
      model.after_save();
      time += timer.GetElapsedSeconds();
    }

    template<typename MODEL>
    void destroy(MODEL& model)
    {
      Utils::Timer timer;

      model.before_destroy();
      start_transaction_for(model);
      try
      {
        model.destroy(transaction.get_database());
	model.after_destroy();
      }
      catch (const odb::object_not_persistent& e)
      {
        ODB::throw_exception(model, e.what());
      }
      time += timer.GetElapsedSeconds();
    }

    template<typename MODEL>
    void destroy(odb::query<MODEL> query = odb::query<MODEL>(true))
    {
      odb::result<MODEL> results;

      if (find(results, query))
      {
        for (auto& model : results)
          destroy(model);
      }
    }

    Transaction transaction;
  };
}

#endif
