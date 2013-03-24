#ifndef  HELPERS_HPP
# define HELPERS_HPP

# define ref_attr_getter(type, name, attr) \
const type& Get##name(void) const { return (attr); } \
type&       Get##name(void)       { return (attr); } \

# define ref_attr_setter(type, name, attr) \
void Set##name(const type& attr) { this->attr = attr; }

# define ref_attr_accessor(type, name, attr) ref_attr_getter(type, name, attr) ref_attr_setter(type, name, attr)

# define attr_getter(type, name, attr) \
type Get##name(void) const { return (attr); }

# define attr_setter(type, name, attr) \
void Set##name(type attr) { this->attr = attr; }

# define attr_accessor(type, name, attr) attr_getter(type, name, attr) attr_setter(type, name, attr)

# define foreach(array, lambda) std::for_each(array.begin(), array.end(), lambda)

// ENABLE IF
# include <type_traits>
/*template<bool C, typename T = void>
struct enable_if { typedef T type; };

template<typename T>
struct enable_if<false, T> { };

template<typename, typename> struct is_same       { static bool const value = false; };

template<typename A>         struct is_same<A, A> { static bool const value = true; };

template<typename B, typename D>                                 
struct is_base_of
{                                                       
    static D * create_d();                     
    static char (& chk(B *))[1]; 
    static char (& chk(...))[2];           
    static bool const value = sizeof chk(create_d()) == 1 &&
                              !is_same<B    volatile const, 
                                       void volatile const>::value;
};*/
// ENABLE IF


#endif
