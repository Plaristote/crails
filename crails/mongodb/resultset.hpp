#ifndef  MONGODB_RESULTSET_HPP
# define MONGODB_RESULTSET_HPP

# include <mongo/client/dbclient.h>
# include <functional>
# include <Boots/Utils/smart_pointer.hpp>
# include "collection.hpp"
# include "model.hpp"
# include "database.hpp"
# include "crails/databases.hpp"

namespace MongoDB
{
  class Collection;

  template<typename MODEL>
  class ResultSet
  {
  public:
    ResultSet(Collection& collection, mongo::Query query) : collection(collection),
    n_to_skip(0), n_to_return(0), query_options(0),
    batch_size(0), fields_to_return(0), query(query)
    {
    }

    void set_skip(int n_to_skip)                      { this->n_to_skip        = n_to_skip;     }
    void set_limit(int n_to_return)                   { this->n_to_return      = n_to_return;   }
    void set_fields_to_return(mongo::BSONObj* fields) { this->fields_to_return = fields;        }
    void set_query_options(int query_options)         { this->query_options    = query_options; }
    void set_batch_size(int batch_size)               { this->batch_size       = batch_size;    }

    void ensure_result_fetched()
    {
      if (results.Null())
        fetch();
    }

    void fetch()
    {
      typename std::auto_ptr<mongo::DBClientCursor> results_auto_ptr;

      results_auto_ptr = collection.query(query,
                                          n_to_return,
                                          n_to_skip,
                                          fields_to_return,
                                          query_options,
                                          batch_size);
      this->results    = results_auto_ptr.get();
      results_auto_ptr.release();
    }

    static SmartPointer<ResultSet> prepare(Collection& collection, mongo::Query query = mongo::Query())
    {
      return (new ResultSet(collection, query));
    }
    
    static SmartPointer<ResultSet> prepare(mongo::Query query = mongo::Query())
    {
      const std::string database        = MODEL::DatabaseName();
      const std::string collection_name = MODEL::CollectionName();
      Collection&       collection      = Crails::Databases::singleton::Get()->get_database<MongoDB::Database>(database)[collection_name];

      return (prepare(collection, query));
    }

    void each(std::function<bool (MODEL&)> functor)
    {
      ensure_result_fetched();
      {
        typename std::list<MODEL>::iterator it  = fetched.begin();
        typename std::list<MODEL>::iterator end = fetched.end();

        for (; it != end ; ++it)
        {
          if (!(functor(*it)))
            return ;
        }
        while (results->more())
        {
          mongo::BSONObj object = results->next();
          MODEL          model(collection, object);

          model.result_set = results;
          model.initialize_fields();
          fetched.push_back(model);
          if (!(functor(*fetched.rbegin())))
            break ;
        }
      }
    }

    std::list<MODEL>& entries(void)
    {
      each([](MODEL&) -> bool { return (true); });
      return (fetched);
    }

    unsigned int count(void)
    {
      if (results.Null())
        return collection.count(query);
      return entries().size();
    }

    void remove(void)
    {
      collection.remove(query, false);
    }
    
    void remove_one(void)
    {
      collection.remove(query, true);
    }

  private:
    Collection&                          collection;
    int                                  n_to_skip, n_to_return, query_options, batch_size;
    mongo::BSONObj*                      fields_to_return;
    mongo::Query                         query;

    SmartPointer<mongo::DBClientCursor>  results;
    std::list<MODEL>                     fetched;

    std::string                          foreign_key;
    mongo::OID                           foreign_oid;
  };  
}

#endif
