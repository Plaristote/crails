#ifndef  HELPERS_HPP
# define HELPERS_HPP

# define ref_attr_getter(type, attr) \
const type& get_##attr(void) const { return (attr); } \
type&       get_##attr(void)       { return (attr); } \

# define ref_attr_setter(type, attr) \
void set_##attr(const type& attr) { this->attr = attr; }

# define ref_attr_accessor(type, name, attr) ref_attr_getter(type, attr) ref_attr_setter(type, attr)

# define const_attr_getter(type, attr) \
const type get_##attr(void) const { return (attr); }

# define attr_getter(type, attr) \
type get_##attr(void) const { return (attr); }

# define attr_setter(type, attr) \
void set_##attr(const type attr) { this->attr = attr; }

# define attr_accessor(type, attr) attr_getter(type, attr) attr_setter(type, attr)

# define const_attr_accessor(type, attr) \
const_attr_getter(type, attr) \
attr_setter(type, attr)

# include <type_traits>

#endif
