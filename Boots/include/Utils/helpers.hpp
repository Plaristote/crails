#ifndef  HELPERS_HPP
# define HELPERS_HPP

# define ref_attr_getter(type, name, attr) \
const type& Get##name(void) const { return (attr); } \
type&       Get##name(void)       { return (attr); } \

# define ref_attr_setter(type, name, attr) \
void Set##name(const type& attr) { this->attr = attr; }

# define ref_attr_accessor(type, name, attr) ref_attr_getter(type, name, attr) ref_attr_setter(type, name, attr)

# define const_attr_getter(type, name, attr) \
const type Get##name(void) const { return (attr); }

# define attr_getter(type, name, attr) \
type Get##name(void) const { return (attr); }

# define attr_setter(type, name, attr) \
void Set##name(const type attr) { this->attr = attr; }

# define attr_accessor(type, name, attr) attr_getter(type, name, attr) attr_setter(type, name, attr)

# define const_attr_accessor(type, name, attr) \
const_attr_getter(type,name,attr) \
attr_setter(type, name, attr)

# define foreach(array, lambda) std::for_each(array.begin(), array.end(), lambda)

# include <type_traits>

#endif
