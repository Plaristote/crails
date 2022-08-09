#ifndef  PAGINATOR_HPP
# define PAGINATOR_HPP

# ifdef WITH_ODB
#  include <odb/query.hxx>
# endif
# ifdef WITH_MONGODB
#  include <mongocxx/pipeline.hpp>
# endif
# include <crails/datatree.hpp>
# include <crails/shared_vars.hpp>

class Paginator
{
  static const unsigned int max_items_per_page;
  static const unsigned int default_items_per_page;
public:
  Paginator(Data params) : params(params)
  {
  }

# ifdef WITH_ODB
  template<typename MODEL>
  void decorate_query(odb::query<MODEL>& query) const
  {
    if (enabled)
    {
      query = MODEL::default_order_by(query)
        + "LIMIT"  + odb::query<MODEL>::_val(get_items_per_page())
        + "OFFSET" + odb::query<MODEL>::_val(get_current_offset());
    }
  }
# endif

# ifdef WITH_MONGODB
  void decorate_query(mongocxx::pipeline& pipe) const
  {
    pipe.skip(get_current_offset());
    pipe.limit(get_items_per_page());
  }
# endif

  void decorate_view(Crails::SharedVars& vars, std::function<unsigned long ()> get_total_items) const
  {
    vars["with_paginator"] = enabled;
    if (enabled)
    {
      vars["page"]           = get_current_page();
      vars["items_per_page"] = get_items_per_page();
      vars["items_count"]    = get_total_items();
    }
  }

  void set_enabled(bool enabled) { this->enabled = enabled; }
  bool is_enabled() const { return enabled; }
  unsigned int get_items_per_page() const;
  unsigned int get_current_offset() const;
  unsigned int get_current_page() const;

private:
  bool enabled = true;
  Data params;
};

#endif
