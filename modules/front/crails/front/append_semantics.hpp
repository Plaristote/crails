#ifndef  CRAILS_FRONT_APPEND_SEMANTICS_HPP
# define CRAILS_FRONT_APPEND_SEMANTICS_HPP

# include <cheerp/clientlib.h>
# include <vector>
# include <list>
# include <memory>
# include <type_traits>

/*
 * Type solving tools for the `inner` and `operator>` methods of Crails::Front::Element
 * - support std::list and std::vector containers, or passing by value
 * - support pointer and non-pointer value types (including shared_ptr)
 */
namespace Crails
{
  namespace Front
  {
    class Element;

    template<typename Test, template<typename...> class Ref>
    struct is_specialization : std::false_type {};

    template<template<typename...> class Ref, typename... Args>
    struct is_specialization<Ref<Args...>, Ref>: std::true_type {};    
    
    template<typename Test>
    struct is_array_container
    {
      static const bool value = is_specialization<Test, std::vector>::value || is_specialization<Test, std::list>::value;
    };

    template<typename ELEMENT, bool is_pointer>
    struct ElementAppender
    {
      static void append_a_to_b(const ELEMENT& a, client::HTMLElement* b)
      {
        b->appendChild(*a);
      }
    };

    template<typename ELEMENT>
    struct ElementAppender<ELEMENT, true>
    {
      static void append_a_to_b(const ELEMENT& a, client::HTMLElement* b)
      {
        b->appendChild(**a);
      }
    };

    template<typename CONTAINER, bool is_array>
    struct ElementListAppender
    {
      static void append_list(const CONTAINER& container, client::HTMLElement* ptr)
      {
        constexpr bool is_ptr = std::is_pointer<typename CONTAINER::value_type>::value || std::is_same<typename CONTAINER::value_type, std::shared_ptr<Crails::Front::Element> >::value;

        for (const auto& i : container)
          ElementAppender<typename CONTAINER::value_type, is_ptr>::append_a_to_b(i, ptr);
      }
    };

    template<typename CONTAINER>
    struct ElementListAppender<CONTAINER, false>
    {
      static void append_list(const CONTAINER& container, client::HTMLElement* ptr)
      {
        constexpr bool is_ptr = std::is_pointer<CONTAINER>::value || std::is_same<CONTAINER, std::shared_ptr<Crails::Front::Element> >::value;

        ElementAppender<CONTAINER, is_ptr>::append_a_to_b(container, ptr);
      }
    };
  }
}

#endif
