#ifndef  FRONT_APPLICATION_CONTROLLER_HPP
# define FRONT_APPLICATION_CONTROLLER_HPP

# include <crails/front/mvc/controller.hpp>

class ApplicationController : public Crails::Front::Controller
{
  struct HomeView : public Crails::Front::View
  {
    HomeView()
    {
      *this > Crails::Front::Element("p").text("Hello world !");
    }
  };

public:
  ApplicationController(const Crails::Front::Params& params) : Crails::Front::Controller(params)
  {
    set_layout("application");
  }

  Crails::Front::Promise home()
  {
    auto view = std::make_shared<HomeView>();

    render(view);
    return Crails::Front::Promise::solved_promise();
  }
};

#endif
