#ifndef  CRAILS_FRONT_MVC_CONTROLLER_HPP
# define CRAILS_FRONT_MVC_CONTROLLER_HPP

# include <crails/front/promise.hpp>
# include <crails/front/http.hpp>
# include <crails/front/router.hpp>
# include <crails/odb/id_type.hpp>
# include <sstream>
# include <map>
# include "layout.hpp"
# include "view.hpp"

namespace Crails
{
  namespace Front
  {
    class Controller
    {
      static std::map<std::string, std::shared_ptr<Layout> > layouts;
      static std::string current_layout;
    public:
      Controller(const Params& p) : params(p)
      {
      }

      Crails::Front::Promise initialize() { return Promise::solved_promise(); }
      Crails::Front::Promise finalize()   { return Promise::solved_promise(); }

      void render(std::shared_ptr<View>);

      static void set_layout(const std::string&);
      static void register_layout(const std::string& name, std::shared_ptr<Layout> object) { layouts.emplace(name, object); }

    protected:
      Params params;

    private:
      static void attach_layout(const std::string&);
      static void detach_layout();
    };

    template<typename MODEL, typename SUPER = Crails::Front::Controller>
    class ModelController : public SUPER
    {
    public:
      ModelController(const Params& p) : SUPER(p)
      {
      }

    protected:
      Crails::Front::Promise find_model()
      {
        std::stringstream stream;
        ODB::id_type id;

        stream << SUPER::params.at("id");
        stream >> id;
        model = std::make_shared<MODEL>();
        model->set_id(id);
        return model->fetch();
      }

      Crails::Front::Promise find_models()
      {
        auto request = Http::Request::get(MODEL().get_url());

        return request->send().then([this, request]()
        {
          auto response = request->get_response();

          if (response->ok())
          {
            if (response->has_body())
            {
              MODEL model;
              DataTree tree = response->get_response_data();
              Data data = tree[model.get_resource_name() + 's'];

              data.each([this](Data model_data) -> bool
              {
                auto new_model = std::make_shared<MODEL>();

                new_model->parse(model_data.to_json());
                models.push_back(new_model);
                return true;
              });
            }
          }
        });
      }

      std::shared_ptr<MODEL>               model;
      std::vector<std::shared_ptr<MODEL> > models;
    };
  }
}

#endif
