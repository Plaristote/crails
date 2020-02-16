#ifndef  CRAILS_FRONT_SLOT_ELEMENT_HPP
# define CRAILS_FRONT_SLOT_ELEMENT_HPP

# include "anchorable_element.hpp"
# include "bindable.hpp"

namespace Crails
{
  namespace Front
  {
    class SlotElement : public AnchorableElement
    {
      std::shared_ptr<IBindableView> element;
      IBindableView*                 element_ptr = nullptr;

    public:
      bool has_element() const
      {
        return element_ptr != nullptr;
      }

      IBindableView* get_element() const
      {
        return element_ptr;
      }

      void attach()
      {
        std::vector<Crails::Front::Element*> element_list;

        element_list.push_back(get_element());
        attach_elements(element_list);
      }

      void set_element(IBindableView& el)
      {
        cleanup();
        element_ptr = &el;
        if (is_anchorable())
          attach();
      }

      template<typename ELEMENT>
      void set_element(std::shared_ptr<ELEMENT> pointer)
      {
        cleanup();
        element = std::dynamic_pointer_cast<IBindableView>(pointer);
	element_ptr = element.get();
        if (is_anchorable() && has_element())
          attach();
      }
    private:
      void cleanup()
      {
        if (has_element())
          get_element()->destroy();
	element = nullptr;
        element_ptr = nullptr;
      }
    };
  }
}

#endif
