#include <Boots/Utils/test.hpp>
#include <unistd.h>
#include <Boots/Utils/flyweight.hpp>
#include <iostream>
#include <string>

using namespace std;

class MyFlyweight : public Flyweight<std::string, std::string>
{
public:
  std::string* CreateInstance(std::string key)
  {
    std::string* to_ret = new std::string;

    (*to_ret) = "Instance of " + key;
    return (to_ret);
  }
};

void TestFlyweight(UnitTest& tester)
{
  tester.AddTest("Flyweight", "Must instantiate a required object when it doesn't exist yet", []() -> string
  {
    MyFlyweight       my_flyweight;
    MyFlyweight::Item item;
    std::string*      ptr;

    item = my_flyweight.Require(std::string("item_1"));
    if (*item != "Instance of item_1")
      return ("Didn't instantiate the item properly");
    ptr = &(*item);
    item = my_flyweight.Require(std::string("item_1"));
    if (ptr != &(*item))
      return ("Reinstanciated the item instead of using the existing one");
    return ("");
  });

  tester.AddTest("Flyweight", "Contains must return true/false if the object is already required or not", [] () -> string
  {
    MyFlyweight       my_flyweight;

    if (my_flyweight.Contains(std::string("item")) == true)
      return ("Returned true for an item that wasnt required");
    my_flyweight.Require(std::string("item"));
    if (my_flyweight.Contains(std::string("item")) == false)
      return ("Returned fa;se for an item that was required");
    return ("");
  });

  tester.AddTest("Flyweight", "Must garbage collect unused objects", []() -> string
  {
    MyFlyweight       my_flyweight;
    std::string*      ptr;

    {
      MyFlyweight::Item item;

      item = my_flyweight.Require(std::string("item_1"));
      ptr = &(*item);
    }
    my_flyweight.GarbageCollect();
    {
      MyFlyweight::Item item;

      MyFlyweight::Item item2 = my_flyweight.Require(std::string("item_2")); // Ensure that it won't be allocated on the same adress
      item = my_flyweight.Require(std::string("item_1"));
      if (ptr == &(*item))
        return ("Didn't garbage collect unused object");
    }
    return ("");
  });
}

