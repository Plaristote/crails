#include <Utils/test.hpp>
#include <Utils/datatree.hpp>

using namespace std;

void TestsJSON(UnitTest& tester)
{
  //
  // Testing JSON Parser
  //
  tester.AddTest("JSON", "Numeric values", []() -> string
  {
    string    str  = "{ \"num1\": 12, \"num2\": 8 }";
    DataTree* tree = DataTree::Factory::StringJSON(str);

    if (tree)
    {
      Data data(tree);

      if (data["num1"] == 12 && data["num2"] == 8)
        return ("");
      return ("Bad parsing");
    }
    return ("JSON failed to load the string");
  });

  tester.AddTest("JSON", "Arrays", []() -> string
  {
    string    str  = "[ 1, 2, 3, 4, 5 ]";
    DataTree* tree = DataTree::Factory::StringJSON(str);

    if (tree)
    {
      Data data(tree);

      if (data[0] == 1 && data[4] == 5)
        return ("");
      return ("Bad parsing");
    }
    return ("Parser failed to load the string");
  });

  tester.AddTest("JSON", "Objects within objects", []() -> string
  {
    string    str  = "{ \"obj1\": { \"obj2\": { \"obj3\": 42 } }, \"obj4\": 21 }";
    DataTree* tree = DataTree::Factory::StringJSON(str);

    if (tree)
    {
      Data data(tree);

      if (data["obj1"]["obj2"]["obj3"] == 42 && data["obj4"] == 21)
        return ("");
      return ("Bad parsing");
    }
    return ("Parser failed to load the string");
  });

  tester.AddTest("JSON", "Line returns in strings", []() -> string
  {
    string    str  = "{ \"key\": \"value\n\nnewline\n\" }";
    DataTree* tree = DataTree::Factory::StringJSON(str);

    if (tree)
    {
      Data data(tree);

      if (data["key"].Value() == "value\n\nnewline\n")
        return ("");
      return ("Bad parsing");
    }
    return ("Parser failed to load the string");
  });

  tester.AddTest("JSON", "Writing JSON strings", []() -> string
  {
    string    str  = "{ \"obj1\": { \"obj2\": { \"obj3\": 42 } }, \"obj4\": 21 }";
    DataTree* tree = DataTree::Factory::StringJSON(str);

    if (tree)
    {
      string  stringjson;

      if ((DataTree::Writers::StringJSON(tree, stringjson)))
      {
        DataTree* subtree = DataTree::Factory::StringJSON(stringjson);

        if (subtree)
        {
          Data data(subtree);

          if (data["obj4"] == 21 && data["obj1"]["obj2"]["obj3"] == 42)
            return ("");
          return ("JSON writer didn't keep the integrity of the JSON document");
        }
        return ("JSON writer wrote an invalid JSON document");
      }
      return ("Writer failed to write the JSON file");
    }
    return ("Parser failed to load the string");
  });
}

