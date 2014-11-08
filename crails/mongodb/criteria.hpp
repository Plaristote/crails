#ifndef  MONGODB_CRITERIA_HPP
# define MONGODB_CRITERIA_HPP

# include "resultset.hpp"
# include <map>
# include <string>
# include <algorithm>

namespace MongoDB
{
  template<typename MODEL>
  class Criteria
  {
  public:
    Criteria() :
      database_name(MODEL::DatabaseName()),
      collection_name(MODEL::CollectionName()),
      collection(Databases::singleton::Get()->GetDb<MongoDB::Database>(database_name)[collection_name])
    {
      n_limit = n_skip = 0;
      sort_by_field = "";
    }

    Criteria& where(const std::map<std::string,std::string>& criterias)
    {
      reset_entries();
      std::for_each(criterias.begin(), criterias.end(), [this](std::pair<std::string,std::string> values)
      {
        builder.appendElements(QUERY(values.first << values.second).getFilter());
      });
      return (*this);
    }

    /*Criteria& where(const std::string& key, const std::string& op, const std::string& value)
    {
      reset_entries();
      builder.appendElements(QUERY(key << GT << value).getFilter());
      return (*this);
    }*/

    Criteria& sort_by(const std::string& field)
    {
      reset_entries();
      sort_by_field = field;
      return (*this);
    }

    Criteria& limit(int n_limit)
    {
      reset_entries();
      this->n_limit = n_limit;
      return (*this);
    }

    Criteria& skip(int n_skip)
    {
      reset_entries();
      this->n_skip = n_skip;
      return (*this);
    }

    std::list<MODEL>& entries()
    {
      require_entries();
      return (resultset->Entries());
    }

    void each(std::function<bool (MODEL&)> functor)
    {
      require_entries();
      resultset->Each(functor);
    }

  private:
    void reset_entries()
    {
      resultset = SmartPointer<ResultSet<MODEL> >();
    }

    void require_entries()
    {
      if (resultset.Null())
      {
        mongo::Query query(builder.obj());

        query.sort(sort_by_field);
        resultset = ResultSet<MODEL>::Query(collection, query, n_limit, n_skip);
      }
    }

    const std::string               database_name;
    const std::string               collection_name;
    MongoDB::Collection&            collection;
    mongo::BSONObjBuilder           builder;
    SmartPointer<ResultSet<MODEL> > resultset;
    int                             n_limit, n_skip;
    std::string                     sort_by_field;
  };
}

#endif
