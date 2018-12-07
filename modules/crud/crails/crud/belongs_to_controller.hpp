#ifndef  BELONGS_TO_CONTROLLER_HPP
# define BELONGS_TO_CONTROLLER_HPP

# include <crails/params.hpp>
# include <crails/odb/id_type.hpp>
# include <memory>
# include <string>
# include "controller.hpp"

namespace Crud
{
  template<typename BASE, typename RESOURCE, typename PARENT = RESOURCE>
  class BelongsToController : public Crud::Controller<BASE, RESOURCE>
  {
    typedef Crud::Controller<BASE, RESOURCE> Super;
  public:
    BelongsToController(Crails::Params& params) : Super(params)
    {}

    void initialize()
    {
      Super::initialize();
      initialize_parent_resource();
    }

    void initialize_parent_resource()
    {
      if (has_parent_id_param())
      {
        auto parent_id = Super::params["parent_id"].template as<Db::id_type>();
        auto query = odb::query<PARENT>::id == parent_id;

        if (!(Super::database.find_one(parent_resource, query)))
          Super::response["status"] = Super::ResponseStatus::not_found;
      }
    }

    bool has_parent_id_param() const
    {
      std::string parent_id = Super::params["parent_id"].template defaults_to<std::string>("root");

      return parent_id != "all" && parent_id != "root";
    }

  protected:
    std::shared_ptr<PARENT> parent_resource;
  };
}

#endif
