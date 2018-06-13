#ifndef DATATREE_TRAITS_HPP
#define DATATREE_TRAITS_HPP
 
#include <cstring> // std::memcpy
#include <odb/pgsql/traits.hxx>
#include <crails/datatree.hpp>
#include <crails/logger.hpp>

namespace odb
{
  namespace pgsql
  {
    template <>
    class value_traits<DataTree, id_string>
    {
    public:
      typedef DataTree value_type;
      typedef value_type query_type;
      typedef details::buffer image_type;
 
      static void
      set_value (DataTree& tree,
                 const details::buffer& b,
                 std::size_t n,
                 bool is_null)
      {
        if (!is_null && n > 0)
        {
          std::string v;
          v.assign (b.data (), n);
          try {
            tree.from_json(v);
          } catch (const std::exception& e) {
            Crails::logger << "[ODB][DataTree traits] could not unserialize: " << e.what() << Crails::Logger::endl;
          }
        }
        else
          tree.as_data().destroy();
      }
 
      static void
      set_image (details::buffer& b,
                 std::size_t& n,
                 bool& is_null,
                 const DataTree& tree)
      {
        std::string v = tree.to_json();

        is_null = false;
        n = v.size ();
 
        if (n > b.capacity ())
          b.capacity (n);
 
        std::memcpy (b.data (), v.c_str (), n);
      }
    };
  }
}

#endif
