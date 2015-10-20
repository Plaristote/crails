#include <crails/mongodb/criteria.hpp>

using namespace MongoDB;
using namespace std;

CriteriaBase::CriteriaBase(Collection& collection, mongo::Query query) : collection(collection),
query_options(0), batch_size(0), sort_order(0), fields_to_return(0), query(query)
{
}

unsigned int CriteriaBase::count(void)
{
  return collection.count(query);
}

void CriteriaBase::where(mongo::Query new_query)
{
  mongo::BSONObjBuilder query_builder;

  query_builder.appendElements(query.getFilter());
  query_builder.appendElements(new_query.getFilter());
  query = mongo::Query(query_builder.obj());
}

void CriteriaBase::sort_by(const string& key, char value)
{
  sort_key = key;
  sort_order = value;
}

void CriteriaBase::remove(void)
{
  collection.remove(query, false);
}

void CriteriaBase::remove_one(void)
{
  collection.remove(query, true);
}

void CriteriaBase::ensure_result_fetched()
{
  if (results.Null())
    fetch();
}

void CriteriaBase::fetch()
{
  typename std::auto_ptr<mongo::DBClientCursor> results_auto_ptr;

  if (sort_order != 0)
    query = query.sort(sort_key, sort_order);
  results_auto_ptr = collection.query(query,
                                      n_to_return,
                                      n_to_skip,
                                      fields_to_return,
                                      query_options,
                                      batch_size);
  this->results    = results_auto_ptr.get();
  results_auto_ptr.release();
}
