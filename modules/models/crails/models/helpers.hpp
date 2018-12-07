#ifndef  MODEL_HELPERS_HPP
# define MODEL_HELPERS_HPP

# include <vector>
# include <list>
# include <memory>
# include <crails/datatree.hpp>
# include <crails/odb/id_type.hpp>
# ifndef ODB_COMPILER
#  include <crails/odb/connection.hpp>
# endif

#ifndef ODB_COMPILER
namespace odb
{
  template<typename MODEL, typename CONTAINER = MODEL>
  std::vector<MODEL> to_vector(odb::result<CONTAINER>& results)
  {
    std::vector<MODEL> models;
    unsigned int i = 0;

    models.resize(results.size());
    for (const auto& result : results)
      models[i++] = result;
    return models;
  }
}
#endif

template<typename LIST>
LIST my_unique(const LIST& list)
{
  LIST new_list;

  for (auto it = list.begin() ; it != list.end() ; ++it)
  {
    auto exists = std::find(new_list.begin(), new_list.end(), *it);

    if (exists == new_list.end())
      new_list.push_back(*it);
  }
  return new_list;
}

template<typename MODELS>
std::vector<ODB::id_type> collect_ids_from(const MODELS& models)
{
  std::vector<ODB::id_type> ids;

  for (auto model : models)
  {
    if (model)
      ids.push_back(model->get_id());
  }
  return ids;
}

# ifndef CRAILS_FRONT_HELPERS
template<typename MODEL>
bool update_id_list(
  std::vector<ODB::id_type>& model_list,
  Data model_ids)
{
#  ifndef ODB_COMPILER
  auto ids = my_unique<std::vector<ODB::id_type> >(model_ids);

  for (auto it = model_list.begin() ; it != model_list.end() ;)
  {
    auto exists_in_new_list = find(ids.begin(), ids.end(), *it);

    if (exists_in_new_list == ids.end())
      it = model_list.erase(it);
    else
    {
      ids.erase(exists_in_new_list);
      it++;
    }
  }

  for (ODB::id_type id : ids)
  {
    std::shared_ptr<MODEL> model;

    if (!ODB::Connection::instance->find_one(model, odb::query<MODEL>::id == id))
      return false;
    model_list.push_back(id);
  }
#  endif
  return true;
}

template<typename MODEL>
bool update_id_list(
  std::list<std::shared_ptr<MODEL> >& model_list,
  Data model_ids)
{
#  ifndef ODB_COMPILER
  std::vector<ODB::id_type> ids = model_ids;
  {
    auto it = model_list.begin();

    while (it != model_list.end())
    {
      std::shared_ptr<MODEL> model(*it);
      auto               exists_in_new_list = find(ids.begin(), ids.end(), model->get_id());

      if (exists_in_new_list == ids.end())
        it = model_list.erase(it); // if it isn't in the new list, remove the building
      else
      {
        ids.erase(exists_in_new_list); // if it is in both lists, ignore it
        it++;
      }
    }
  }

  for (ODB::id_type id : ids)
  {
    std::shared_ptr<MODEL> model;

    if (!ODB::Connection::instance->find_one(model, odb::query<MODEL>::id == id))
      return false;
    model_list.push_back(model);
  }
#  endif
  return true;
}
# endif

#endif
