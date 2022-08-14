#include "paginator.hpp"

const unsigned int Paginator::max_items_per_page     = 300;
const unsigned int Paginator::default_items_per_page = 100;

unsigned int Paginator::get_items_per_page() const
{
  unsigned int items_per_page = params["count"].defaults_to<unsigned short>(default_items_per_page);

  return std::min(items_per_page, max_items_per_page);
}

unsigned int Paginator::get_current_page() const
{
  return params["page"].defaults_to<unsigned short>(0);
}

unsigned int Paginator::get_current_offset() const
{
  return get_items_per_page() * get_current_page();
}
