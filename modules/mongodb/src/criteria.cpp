#include <crails/mongodb/criteria.hpp>

using namespace MongoDB;
using namespace std;

CriteriaBase::CriteriaBase(Collection& collection, mongo::Query query) : collection(collection),
n_to_skip(0), n_to_return(0), query_options(0),
batch_size(0), fields_to_return(0), query(query)
{
}

unsigned int CriteriaBase::count(void)
{
  return collection.count(query);
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

  results_auto_ptr = collection.query(query,
                                      n_to_return,
                                      n_to_skip,
                                      fields_to_return,
                                      query_options,
                                      batch_size);
  this->results    = results_auto_ptr.get();
  results_auto_ptr.release();
}
