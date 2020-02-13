#ifndef  CRAILS_FRONT_REPEATER_HPP
# define CRAILS_FRONT_REPEATER_HPP

# include "anchorable_element.hpp"

namespace Crails
{
  namespace Front
  {
    template<typename ARRAY, typename ELEMENT>
    struct Repeater : public AnchorableElement
    {
      typedef typename ARRAY::const_iterator                 Iterator;
      typedef std::pair<Iterator, std::shared_ptr<ELEMENT> > ElementPair;
      typedef std::vector<ElementPair>                       Elements;
      Elements elements;

      template<typename PARENT>
      void refresh(PARENT* parent, const ARRAY& array)
      {
        purge_removed_elements(array);
        update_elements(parent, array);
        attach_elements();
        trigger_binding_updates();
      }

      void bind_attributes()
      {
        for (const auto pair : elements)
          pair.second->bind_attributes();
      }

      void trigger_binding_updates()
      {
        for (const auto pair : elements)
          pair.second->trigger_binding_updates();
      }

    private:
      void attach_elements()
      {
        std::vector< std::shared_ptr<ELEMENT> > element_vector;

	element_vector.reserve(elements.size());
	for (const auto& pair : elements)
          element_vector.push_back(pair.second);
	attach_elements(element_vector);
      }

      void purge_removed_elements(const ARRAY& array)
      {
        std::vector<typename Elements::iterator> to_remove;

        for (auto it = elements.begin() ; it != elements.end() ; ++it)
        {
          bool exists = false;
          for (auto subit = array.begin() ; subit != array.end() ; ++subit)
          {
            if (subit == it->first)
            {
              exists = true;
              break ;
            }
          }
          if (!exists)
            to_remove.push_back(it);
        }
        for (auto it : to_remove)
        {
          it->second->destroy();
          elements.erase(it);
        }
      }

      template<typename PARENT>
      void update_elements(PARENT* parent, const ARRAY& array)
      {
        Elements new_elements;

        for (Iterator it = array.begin() ; it != array.end() ; ++it)
        {
          bool exists = false;

          for (auto subit = elements.begin() ; subit != elements.end() ; ++subit)
          {
            if (subit->first == it)
            {
              new_elements.push_back(ElementPair(it, subit->second));
              exists = true;
              break ;
            }
          }
          if (!exists)
          {
            auto ptr = std::make_shared<ELEMENT>(parent, *it);

            new_elements.push_back(ElementPair(it, ptr));
          }
        }
        elements = new_elements;
      }
    };
  }
}

#endif
