#ifndef  MONGODB_RESULTSET_HPP
# define MONGODB_RESULTSET_HPP

# include <mongo/client/dbclient.h>
# include <functional>
# include <Boots/Utils/smart_pointer.hpp>
# include "collection.hpp"
# include "model.hpp"
# include "crails/databases.hpp"
# include "../mongodb.hpp"

namespace MongoDB
{
  class Collection;

  template<typename MODEL>
  class ResultSet
  {
  public:
    ResultSet(Collection& collection, typename std::auto_ptr<mongo::DBClientCursor> results) : collection(collection), results(results.get())
    {
      results.release();
    }

    static ResultSet* Query(Collection& collection, mongo::Query query = mongo::Query(), int n_to_return = 0, int n_to_skip = 0, mongo::BSONObj* fields_to_return = 0, int query_options = 0, int batch_size = 0)
    {
      return (new ResultSet(collection, collection.Query(query, n_to_return, n_to_skip, fields_to_return, query_options, batch_size)));
    }
    
    static ResultSet* Query(mongo::Query query = mongo::Query(), int n_to_return = 0, int n_to_skip = 0, mongo::BSONObj* fields_to_return = 0, int query_options = 0, int batch_size = 0)
    {
      const std::string database        = MODEL::DatabaseName();
      const std::string collection_name = MODEL::CollectionName();
      Collection&       collection      = Databases::singleton::Get()->GetDb<MongodbDb>(database)[collection_name];
      
      return (new ResultSet(collection, collection.Query(query, n_to_return, n_to_skip, fields_to_return, query_options, batch_size)));
    }    

    void Each(std::function<bool (MODEL&)> functor)
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
        model.InitializeFields();
        fetched.push_back(model);
        if (!(functor(*fetched.rbegin())))
          break ;
      }
    }

    std::list<MODEL>& Entries(void)
    {
      Each([](MODEL&) -> bool { return (true); });
      return (fetched);
    }
    
    ResultSet& operator<<(Model& model)
    {
      Collection&    table  = model.GetCollection();
      mongo::BSONObj object = BSON("$set" << BSON(foreign_key << foreign_oid));

      std::cout << "[" << model.Id() << "] Set " << foreign_key << " to " << foreign_oid.toString() << std::endl;
      table.Update(object, QUERY("_id" << model.OID()));
      model.Refresh();
      return (*this);
    }

    void                                 SetOwner(Model& model, std::string relation_name)
    {
      foreign_key = relation_name + "_id";
      foreign_oid = model.OID();
    }
    
  private:
    Collection&                          collection;
    SmartPointer<mongo::DBClientCursor>  results;
    std::list<MODEL>                     fetched;
    
    std::string                          foreign_key;
    mongo::OID                           foreign_oid;
  };  
}

#endif
