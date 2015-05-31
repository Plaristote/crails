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

  class ResultSetBase
  {
  public:
    unsigned int count(void);
    void         remove(void);
    void         remove_one(void);

    void set_skip(int n_to_skip)                      { this->n_to_skip        = n_to_skip;     }
    void set_limit(int n_to_return)                   { this->n_to_return      = n_to_return;   }
    void set_fields_to_return(mongo::BSONObj* fields) { this->fields_to_return = fields;        }
    void set_query_options(int query_options)         { this->query_options    = query_options; }
    void set_batch_size(int batch_size)               { this->batch_size       = batch_size;    }

  protected:
    ResultSetBase(Collection& collection, mongo::Query query);

    void ensure_result_fetched();
    void fetch();

    Collection&                          collection;
    int                                  n_to_skip, n_to_return, query_options, batch_size;
    mongo::BSONObj*                      fields_to_return;
    mongo::Query                         query;
    SmartPointer<mongo::DBClientCursor>  results;
  };
  
  template<typename MODEL>
  class ResultSet : public ResultSetBase
  {
  public:
    ResultSet(Collection& collection, mongo::Query query) : ResultSetBase(collection, query)
    {
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
      if (results.NotNull())
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
          if (functor)
          {
            if (!(functor(*fetched.rbegin())))
              break ;
          }
        }
      }
      else
        throw MongoDB::Exception("the collection's query returned a null pointer instead of a DBCursor");
    }
    
    std::list<MODEL>& entries(void)
    {
      ensure_result_fetched();
      if (results.NotNull() && results->more())
      {
        std::function<bool (MODEL&)> empty_functor;

        each(empty_functor);
      }
      return (fetched);
    }

  private:
    std::list<MODEL>                     fetched;

    std::string                          foreign_key;
    mongo::OID                           foreign_oid;
  };  
}

#endif
