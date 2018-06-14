#ifndef  SYNC_TRANSACTION_HPP
# define SYNC_TRANSACTION_HPP

# include <crails/datatree.hpp>
# include <crails/shared_vars.hpp>
# include <crails/renderer.hpp>
# include <boost/lexical_cast.hpp>

namespace Sync
{
  struct IUpdate
  {
    virtual void               render(Data)       = 0;
    virtual const std::string& scope()      const = 0;
    virtual std::string        uid()        const = 0;
  };

  template<typename T>
  struct Update : public IUpdate
  {
    T object;

    const std::string& scope() const { return T::scope; }

    std::string uid() const
    {
      return scope() + '.' + boost::lexical_cast<std::string>(object.get_id());
    }

    void render(Data data)
    {
      if (T::view != "")
      {
        DataTree           params, response;
        Crails::SharedVars vars;

        params["headers"]["Accept"] = "application/json";
        vars["model"] = &object;
        Crails::Renderer::render(T::view, params.as_data(), response.as_data(), vars);
        data[uid()] = response["body"].as<std::string>();
      }
    }
  };

  class Transaction
  {
    typedef std::list<std::shared_ptr<IUpdate> > UpdateList;
  public:
    static bool is_enabled();
    static std::function<void (DataTree&)> on_commit;

    void commit();
    void rollback();

    template<typename MODEL>
    void save(MODEL& model)
    {
      if (is_enabled())
      {
        auto update = std::make_shared<Update<MODEL> >();

        update->object = model;
        updates.push_back(update);
      }
    }

    template<typename MODEL>
    void destroy(MODEL& model)
    {
      if (is_enabled())
      {
        auto update = std::make_shared<Update<MODEL> >();

        update->object = model;
        removals.push_back(update);
      }
    }

  private:
    UpdateList updates, removals;
  };
}

#endif
