#ifndef  CRAILS_FRONT_REPEATER_HPP
# define CRAILS_FRONT_REPEATER_HPP

namespace Crails
{
  namespace Front
  {
    enum AnchorMode
    {
      ChildrenAnchor,
      AppendAnchor,
      PrependAnchor
    };

    template<typename ARRAY, typename ELEMENT>
    struct Repeater
    {
      Crails::Front::Element anchor;
      AnchorMode             anchor_mode;

      typedef typename ARRAY::const_iterator                 Iterator;
      typedef std::pair<Iterator, std::shared_ptr<ELEMENT> > ElementPair;
      typedef std::vector<ElementPair>                       Elements;
      Elements elements;

      void set_anchor(Crails::Front::Element el, AnchorMode mode)
      {
        anchor = el;
        anchor_mode = mode;
      }

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
        switch (anchor_mode)
        {
        case ChildrenAnchor:
          for (auto& pair : elements)
            pair.second->append_to(anchor);
          break ;
        case AppendAnchor:
          {
            auto current_anchor = anchor;
            for (auto it = elements.begin() ; it != elements.end() ; ++it)
            {
              it->second->insert_after(current_anchor);
              current_anchor = Crails::Front::Element(*it->second);
            }
          }
          break ;
        case PrependAnchor:
          {
            auto current_anchor = anchor;
            for (auto it = elements.rbegin(); it != elements.rend() ; ++it)
            {
              it->second->insert_before(current_anchor);
              current_anchor = Crails::Front::Element(*it->second);
            }
          }
          break ;
        }
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
