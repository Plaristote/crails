#include <Boots/Utils/test.hpp>
#include <Boots/Utils/datatree.hpp>

using namespace std;

void TestsData(UnitTest& tester)
{
  tester.AddTest("Data", "Type morphing", []() -> string
  {
    DataTree tree;
    Data     data(&tree);

    data = "123";
    if (data == 123)
    {
      data = 425.5f;
      if (data.Value() == "425.5")
        return ("");
      return ("Couldn't perform conversion from float to string");
    }
    return ("Couldn't perform conversion from string to int");
  });

  tester.AddTest("Data", "Dynamically add branches", []() -> string
  {
    DataTree tree;
    Data     data(&tree);

    data["test"]["value"] = 12;
    if (data["test"]["value"] == 12)
      return ("");
    return ("test/value didn't remain");
  });

  tester.AddTest("Data", "Removing branches", []() -> string
  {
    DataTree tree;
    Data     data(&tree);

    data["test"] = 12;
    data["test"].Remove();
    if (data["test"].Nil() && (!(data["test"] == 12)))
      return ("");
    return ("test/value remained");
  });

  tester.AddTest("Data", "Duplicate tree", []() -> string
  {
    DataTree tree;
    Data     data(&tree);
    Data     recipient;

    data["value1"] = 1;
    data["children"]["value1"] = 2;
    data["value2"] = 3;
    data["children"]["children"]["value2"] = 4;
    recipient.Duplicate(data);
    if (recipient["value1"] == 1 && recipient["children"]["value1"] == 2 && recipient["value2"] == 3 && recipient["children"]["children"]["value2"] == 4)
      return ("");
    return ("The replica didn't match the replicated tree");
  });

  tester.AddTest("Data", "Garbage collecting", []() -> string
  {
    DataTree tree;
    Data     data(&tree);

    {
      Data tmp = data["coucou"];

      if (tree.children.size() != 1)
        return ("Creating a temporary branch doesn't add it to its parent");
    }
    if (tree.children.size() != 0)
      return ("Temporary branch wasn't removed from its parent when it went out of scope");
    return ("");
  });
}

