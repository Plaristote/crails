#ifndef  CRAILS_FRONT_SLOT_ELEMENT_HPP
# define CRAILS_FRONT_SLOT_ELEMENT_HPP

# include "anchorable_element.hpp"

namespace Crails
{
  namespace Front
  {
    struct SlotElement : public AnchorableElement
    {
      std::shared_ptr<Crails::Front::Element> element;

      void attach()
      {
        std::vector<std::shared_ptr<Crails::Front::Element> > element_list;

	element_list.push_back(element);
        attach_elements(element_list);
      }

      template<typename ELEMENT>
      void set_element(std::shared_ptr<ELEMENT> pointer)
      {
        if (element)
          element->destroy();
        element = std::dynamic_pointer_cast<Crails::Front::Element>(pointer);
        if (is_anchorable())
          attach();
      }
    };
  }
}

#endif
