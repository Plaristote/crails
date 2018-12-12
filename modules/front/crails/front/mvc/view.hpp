#ifndef  FRONT_VIEW_HPP
# define FRONT_VIEW_HPP

# include <crails/front/element.hpp>

namespace Crails
{
  namespace Front
  {
    class Layout;

    class View : public Crails::Front::Element
    {
      friend class Layout;

      virtual void on_attached() {}
    public:
      virtual void render() {}
    };
  }
}

#endif
