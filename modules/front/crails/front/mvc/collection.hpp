#ifndef  FRONTEND_COLLECTION_HPP
# define FRONTEND_COLLECTION_HPP

# include <crails/front/signal.hpp>
# include <crails/front/archive.hpp>
# include <crails/front/mvc/model.hpp>
# include <map>
# include <list>

namespace Crails
{
  namespace Front
  {
    template<typename MODEL>
    class Collection : public Listener
    {
      typedef std::shared_ptr<MODEL>            ModelPtr;
      typedef std::map<unsigned long, ModelPtr> Map;
    public:
      typedef MODEL Model;

      Signal<MODEL&> added;
      Signal<MODEL&> removed;
      Signal<MODEL&> changed;
      Signal<void>   synced;

      virtual std::string get_url() const = 0;

      void add(ModelPtr model)
      {
        auto it = models.at(model->get_id());

        if (it == models.end())
        {
          models.emplace(model->get_id(), model);
          added.trigger(*model);
        }
      }

      void remove(ModelPtr model)
      {
        auto it = models.at(model->get_id());

        if (it != models.end())
        {
          models.erase(it);
          removed.trigger(*model);
        }
      }

      void each(std::function<void (ModelPtr)> callback)
      {
        for (auto item : models)
          callback(item.second);
      }

      std::list<ModelPtr> where(std::function<bool (ModelPtr)> selector) const
      {
        std::list<ModelPtr> results;

        for (auto item : models)
        {
          if (selector(item.second))
            results.push_back(item.second);
        }
        return results;
      }

      ModelPtr find(std::function<bool (ModelPtr)> selector) const
      {
        for (auto item : models)
        {
          if (selector(item.second))
            return item.second;
        }
        return nullptr;
      }

      ModelPtr get(unsigned long id)
      {
        auto it = models.find(id);

        if (it != models.end())
          return it->second;
        return nullptr;
      }

      void fetch(Ajax::Callbacks callbacks = Ajax::Callbacks())
      {
        Ajax::query(get_url())
          .headers({{"Accept",Archive::mimetype}})
          .callbacks(callbacks)
          .on_success([this, callbacks](const Ajax& ajax)
          {
            if (ajax->get_responseText())
              parse((std::string)(*ajax->get_responseText()));
	    if (callbacks.success)
              callbacks.success(ajax);
            synced.trigger();
          })();
      }

      unsigned int count() const
      {
        return models.size();
      }

    protected:
      void parse(const std::string& str)
      {
        IArchive      archive;
        unsigned long size;

        archive.set_data(str);
        archive & size;
        for (unsigned long i = 0 ; i < size ; ++i)
        {
          auto ptr = std::make_shared<MODEL>();
          ptr->serialize(archive);
          models.emplace(ptr->get_id(), ptr);
        }
      }

      Map models;
    };
  }
}

#endif
