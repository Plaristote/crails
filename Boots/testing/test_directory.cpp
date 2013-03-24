#include <Utils/test.hpp>
#include <Utils/directory.hpp>
#include <Utils/datatree.hpp>

using namespace std;

void TestsDirectory(UnitTest& tester)
{
  tester.AddTest("Filesystem", "Creating a directory", []() -> string
  {
    if (Directory::MakeDir("testdir"))
      return ("");
    return ("Directory::MakeDir returned false");
  });
  tester.AddTest("Filesystem", "Removing a directory", []() -> string
  {
    if (Directory::RemoveDir("testdir"))
      return ("");
    return ("Directory::RemoveDir returned false");
  });
  tester.AddTest("Filesystem", "Checking if a file exists", []() -> string
  {
    if (!(Filesystem::FileExists("test")))
      return ("Returned false for an existing file");
    if ((Filesystem::FileExists("test.donotexist")))
      return ("Returned true for a non-existing file");
    return ("");
  });
  tester.AddTest("Filesystem", "Copying a file", []() -> string
  {
    if (!(Directory::MakeDir("testdir")))
      return ("Can't perform test without a working MakeDir");
    if (Filesystem::FileCopy("test", "testdir/test.copy"))
    {
      if (Filesystem::FileExists("test.copy"))
        return ("");
      return ("Failed to properly copy file");
    }
    return ("Filesystem::FileCopy returned false");
  });
  tester.AddTest("Filesystem", "Not removing a directory containing files", []() -> string
  {
    string to_ret;

    if (Directory::RemoveDir("testdir"))
      to_ret = "Directory::RemoveDir returned true";
    remove("testdir/test.copy");
    Directory::RemoveDir("testdir");
    return (to_ret);
  });
}

