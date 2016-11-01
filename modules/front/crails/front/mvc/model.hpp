#ifndef  FRONTEND_MODEL_HPP
# define FRONTEND_MODEL_HPP

# include <crails/front/signal.hpp>
# include <crails/front/archive.hpp>
# include <crails/front/ajax.hpp>

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

namespace Crails
{
  namespace Front
  {
    class Model : public Listener
    {
    public:
      Signal<void> removed;
      Signal<void> synced;

      virtual void serialize(OArchive&) = 0;
      virtual void serialize(IArchive&) = 0;
      virtual unsigned long get_id() const = 0;
      virtual std::string get_url() const = 0;

      void fetch(Ajax::Callbacks callbacks = Ajax::Callbacks());
      void save(Ajax::Callbacks callbacks = Ajax::Callbacks());
      void parse(const std::string& str);
    };
  }
}

#endif
