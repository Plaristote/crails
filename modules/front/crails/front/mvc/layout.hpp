#ifndef  CRAILS_FRONT_LAYOUT_HPP
# define CRAILS_FRONT_LAYOUT_HPP

# include "view.hpp"

namespace Crails
{
  namespace Front
  {
    class Layout : public Crails::Front::Element
    {
      std::shared_ptr<View> current_view;
    public:
      virtual Crails::Front::Element get_content() const = 0;

      void set_current_view(std::shared_ptr<View> view);
    };
  }
}

#endif
