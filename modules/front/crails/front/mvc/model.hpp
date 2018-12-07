#ifndef  FRONTEND_MODEL_HPP
# define FRONTEND_MODEL_HPP

# include <crails/front/signal.hpp>
# include <crails/front/archive.hpp>
# include <crails/front/promise.hpp>
# include <crails/datatree.hpp>

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

      virtual unsigned long get_id() const = 0;
      virtual std::string get_url() const = 0;
      virtual std::string get_resource_name() const { return ""; }

      Promise fetch();
      Promise save();
      virtual void parse(const std::string& str) = 0;

    protected:
      virtual std::string get_payload() = 0;
      virtual std::string get_content_type() const = 0;
    };

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

    class JsonModel : public Model
    {
    public:
      virtual void from_json(Data) = 0;
      virtual std::string to_json() const = 0;
      void parse(const std::string& str);
    protected:
      std::string get_payload();
      std::string get_content_type() const;
    };
  }
}

#endif
