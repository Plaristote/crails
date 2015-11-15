#ifndef  MONGODB_CRITERIA_HPP
# define MONGODB_CRITERIA_HPP

# include <mongo/client/dbclient.h>
# include <functional>
# include <crails/utils/smart_pointer.hpp>
# include "collection.hpp"
# include "model.hpp"
# include "database.hpp"
# include "crails/databases.hpp"

namespace MongoDB
{
  class Collection;

  class CriteriaBase
  {
  public:
    unsigned int count(void);
    void         remove(void);
    void         remove_one(void);
    void         where(mongo::Query);

    void sort_by(const std::string& key, char value = 1);
    void set_skip(int n_to_skip)                      { this->n_to_skip        = n_to_skip;     }
    void set_limit(int n_to_return)                   { this->n_to_return      = n_to_return;   }
    void set_fields_to_return(mongo::BSONObj* fields) { this->fields_to_return = fields;        }
    void set_query_options(int query_options)         { this->query_options    = query_options; }
    void set_batch_size(int batch_size)               { this->batch_size       = batch_size;    }

  protected:
    CriteriaBase(Collection& collection, mongo::Query query);

    void ensure_result_fetched();
    void fetch();

    Collection&                            collection;
    int                                    n_to_skip, n_to_return, query_options, batch_size;
    std::string                            sort_key;
    char                                   sort_order;
    mongo::BSONObj*                        fields_to_return;
    mongo::Query                           query;
    std::unique_ptr<mongo::DBClientCursor> results;
  };

  template<typename MODEL>
  class Criteria : public CriteriaBase
  {
  public:
    Criteria(Collection& collection, mongo::Query query) : CriteriaBase(collection, query)
    {
    }

    static std::unique_ptr<Criteria> prepare(Collection& collection, mongo::Query query = mongo::Query())
    {
      return (std::unique_ptr<Criteria>(new Criteria(collection, query)));
    }

    static std::unique_ptr<Criteria> prepare(mongo::Query query = mongo::Query())
    {
      const std::string database        = MODEL::DatabaseName();
      const std::string collection_name = MODEL::CollectionName();
      Collection&       collection      = CRAILS_DATABASE(MongoDB,database)[collection_name];

      return (prepare(collection, query));
    }

    void each(std::function<bool (MODEL&)> functor)
    {
      ensure_result_fetched();
      if (results)
      {
        bool should_continue = run_for_aggregated_results(functor);

        if (should_continue)
          run_for_unaggregated_results(functor);
      }
      else
        throw MongoDB::Exception("the collection's query returned a null pointer instead of a DBCursor");
    }

    std::list<MODEL>& entries(void)
    {
      ensure_result_fetched();
      if (results && results->more())
      {
        std::function<bool (MODEL&)> empty_functor;

        each(empty_functor);
      }
      return (fetched);
    }

    SP(MODEL) first(void)
    {
      if (fetched.size() == 0 && results->more())
        aggregate_next_result();
      if (fetched.size() > 0)
        return new MODEL(*fetched.begin());
      return NULL;
    }

    SP(MODEL) last(void)
    {
      if (entries().size() > 0)
        return new MODEL(*entries().rbegin());
      return NULL;
    }

  private:
    bool run_for_aggregated_results(std::function<bool (MODEL&)> functor)
    {
      typename std::list<MODEL>::iterator it  = fetched.begin();
      typename std::list<MODEL>::iterator end = fetched.end();

      for (; it != end ; ++it)
      {
        if (!(functor(*it)))
          return false;
      }
      return true;
    }

    void run_for_unaggregated_results(std::function<bool (MODEL&)> functor)
    {
      while (results->more())
      {
        aggregate_next_result();
        if (functor)
        {
          if (!(functor(*fetched.rbegin())))
            break ;
        }
      }
    }

    void aggregate_next_result()
    {
      mongo::BSONObj object = results->next();
      MODEL          model(collection, object);

      model.result_set = results;
      model.initialize_fields();
      fetched.push_back(model);
    }

    std::list<MODEL>                     fetched;
  };

  template<typename MODEL>
  class HasMany : public Criteria<MODEL>
  {
  public:
    HasMany(Collection& collection, std::string& key, mongo::OID oid) :
      Criteria<MODEL>(collection, MONGO_QUERY(key << oid)),
      foreign_key(key),
      foreign_oid(oid)
    {
    }

    HasMany& operator<<(MODEL& model)
    {
      model.set_foreign_id(foreign_key, foreign_oid);
      return *this;
    }

    std::string                          foreign_key;
    mongo::OID                           foreign_oid;
  };
}

#endif
