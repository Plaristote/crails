#ifndef  DYN_STRUCT_HPP
# define DYN_STRUCT_HPP

# include "datatree.hpp"

class DynStruct : public Data
{
public:
  DynStruct(void) : Data(&root) {}
  DynStruct(const DynStruct& other) : Data(&root) { Duplicate(other); }
private:
  DataTree root;
};

#endif
