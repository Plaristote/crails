#include <crails/front/object.hpp>
#include <crails/front/globals.hpp>
#include <iostream>
using namespace Crails::Front;
using namespace std;

static void prepare_is_undefined()
{
  static bool is_prepared = false;
  
  if (!is_prepared)
  {
    client::eval("window.isUndefined = function(a) { return a == undefined ? 'y' : 'n'; };");
    is_prepared = true;
  }
}

bool Object::is_of_type(const char* type) const
{
  if (is_undefined())
    return false;
  return ptr->operator[]("constructor") == *(window[type]);
}

bool Object::is_undefined() const
{
  prepare_is_undefined();
  if (ptr != nullptr)
  {
    Object      answer = window.apply("isUndefined", *this);
    std::string result = (std::string)(*static_cast<client::String*>(answer.ptr));
    return result == "y";
  }
  return true;
}

void Object::set_global(const std::string& key, Object object)
{
  window.set(key, object);
}
