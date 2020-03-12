#ifndef  FRONTEND_MODEL_HPP
# define FRONTEND_MODEL_HPP

# include <comet/mvc/model.hpp>
# include <crails/archive.hpp>

# define add_model_property(type, name) \
  Crails::Signal<type> name##_changed; \
  type get_##name() const { return name; } \
  void set_##name(type val) \
  { \
    bool is_different = name != val; \
    name = val; \
    if (is_different) \
    { \
      changed.trigger(); \
      name##_changed.trigger(val); \
    } \
  }

# define add_model_ref_property(type, name) \
  Crails::Signal<const type&> name##_changed; \
  const type& get_##name() const { return name; } \
  void set_##name(const type& val) \
  { \
    bool is_different = name != val; \
    name = val; \
    if (is_different) \
    { \
      changed.trigger(); \
      name##_changed.trigger(name); \
    } \
  }

namespace Comet
{
  class ArchiveModel : public Model
  {
  public:
    virtual void serialize(OArchive&) = 0;
    virtual void serialize(IArchive&) = 0;
    void parse(const std::string& str);
  protected:
    std::string get_payload();
    std::string get_content_type() const;
  };
}

#endif
