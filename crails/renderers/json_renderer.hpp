#ifndef  JSON_RENDERER_HPP
# define JSON_RENDERER_HPP

# include "../renderer.hpp"
# include <functional>

namespace Crails
{
  class JsonRenderer : public Renderer
  {
  public:
    JsonRenderer();

    bool can_render(const std::string& accept_header, const std::string& view) const;
    void render_template(const std::string& view, Data params, Data response, SharedVars& vars) const;
  };

  class JsonTemplate
  {
  public:
    template<typename T>
    void json(const std::string& key, const T val)
    {
      add_separator();
      add_key(key);
      add_value(val);
    }

    template<typename ITERATOR>
    void json_array(const std::string& key, ITERATOR beg, ITERATOR end)
    {
      add_separator();
      add_key(key);
      stream << '[';
      while (beg != end)
      {
        add_value(*beg);
        ++beg;
      }
      stream << ']';
    }

    template<typename ITERATOR, typename T>
    void json_array(const std::string& key,
                    ITERATOR beg,
                    ITERATOR end,
                    std::function<void (T)> func)
    {
      add_separator();
      add_key(key);
      stream << '[';
      while (beg != end)
      {
        add_object([this, func, beg]() { func(*beg); });
        ++beg;
      }
      stream << ']';
    }

  private:
    template<typename T>
    void        add_value(const T val)
    {
      stream << val;
    }

    void        add_separator();
    void        add_key(const std::string& key);
    void        add_object(std::function<void()> func);
    std::string javascript_escape(std::string val) const;

    bool first_item_in_object;
    std::stringstream stream;
  };
}

#endif
