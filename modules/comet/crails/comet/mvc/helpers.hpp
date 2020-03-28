#ifndef  CRAILS_FRONT_MVC_HELPERS_HPP
# define CRAILS_FRONT_MVC_HELPERS_HPP

# define CRAILS_FRONT_HELPERS
# define ODB_COMPILER
# include <crails/models/helpers.hpp>

template<typename MODEL>
bool update_id_list(
  std::vector<ODB::id_type>& model_list,
  Data model_ids)
{
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
    model_list.push_back(id);
  return true;
}

template<typename MODEL>
bool update_id_list(
  std::list<std::shared_ptr<MODEL> >& model_list,
  Data model_ids)
{
  auto ids = model_ids.to_vector<ODB::id_type>();
  {
    auto it = model_list.begin();

    while (it != model_list.end())
    {
      std::shared_ptr<MODEL> model(*it);
      auto                   exists_in_new_list(find(ids.begin(), ids.end(), model->get_id()));

      if (exists_in_new_list == ids.end())
        it = model_list.erase(it);
      else
      {
        ids.erase(exists_in_new_list);
        it++;
      }
    }
  }

  for (ODB::id_type id : ids)
  {
    std::shared_ptr<MODEL> model;

    model->set_id(id);
    model->fetch();
    model_list.push_back(model);
  }
  return true;
}

#endif
