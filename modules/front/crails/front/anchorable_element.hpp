#ifndef  CRAILS_FRONT_ANCHORABLE_ELEMENT_HPP
# define CRAILS_FRONT_ANCHORABLE_ELEMENT_HPP

# include "element.hpp"

namespace Crails
{
  namespace Front
  {
    enum AnchorMode
    {
      ChildrenAnchor,
      AppendAnchor,
      PrependAnchor,
      AnchorsEnd
    };

    struct AnchorableElement
    {
      Crails::Front::Element anchor;
      AnchorMode             anchor_mode = AnchorsEnd;

      void set_anchor(Crails::Front::Element el, AnchorMode mode)
      {
        anchor = el;
        anchor_mode = mode;
      }

      bool is_anchorable() const
      {
        return anchor_mode != AnchorsEnd;
      }

      template<typename LIST>
      void attach_elements(const LIST& elements)
      {
        switch (anchor_mode)
        {
        case ChildrenAnchor:
          for (auto el : elements)
            el->append_to(anchor);
          break ;
        case AppendAnchor:
          append_to_anchor(elements);
          break ;
        case PrependAnchor:
          prepend_to_anchor(elements);
          break ;
	case AnchorsEnd:
	  __asm__("throw 'AnchorableElement::attach_element called with unset anchor'");
          break ;
        }
      }

    private:
      template<typename LIST>
      void append_to_anchor(const LIST& elements)
      {
        auto current_anchor = anchor;
        for (auto it = elements.begin() ; it != elements.end() ; ++it)
        {
          (*it)->insert_after(current_anchor);
          current_anchor = Crails::Front::Element(*(*it));
        }
      }

      template<typename LIST>
      void prepend_to_anchor(const LIST& elements)
      {
        auto current_anchor = anchor;
        for (auto it = elements.rbegin() ; it != elements.rend() ; ++it)
        {
          (*it)->insert_before(current_anchor);
          current_anchor = Crails::Front::Element(*(*it));
        }
      }
    };
  }
}

#endif
