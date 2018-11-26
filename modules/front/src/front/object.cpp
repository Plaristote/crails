#include <crails/front/object.hpp>
#include <crails/front/globals.hpp>
#include <iostream>
using namespace Crails::Front;
using namespace std;

bool Object::is_of_type(const char* type) const
{
  if (is_undefined())
    return false;
  return ptr->operator[]("constructor") == *(window[type]);
}

bool Object::is_undefined() const
{
  if (ptr != nullptr)
  {
    __asm__("window.isUndefined = function(a) { return a == undefined ? 'y' : 'n'; };");
    Object      answer = window.apply("isUndefined", *this);
    std::string result = (std::string)(*static_cast<client::String*>(answer.ptr));
    return result == "y";
  }
  return true;
}
