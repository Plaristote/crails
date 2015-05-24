#include <Boots/Utils/test.hpp>
#include <Boots/Utils/datetime.hpp>
#include <iostream>
#include <ctime>

using namespace std;

void TestDateTime(UnitTest& tester)
{
  tester.AddTest("DateTime", "casting to unsigned int should return the timestamp", []() -> string
  {
    time_t   timestamp = std::time(0);
    DateTime datetime(timestamp);

    if (timestamp != (int)datetime)
      return ("timestamps didn't match");
    return ("");
  });
  tester.AddTest("DateTime", "DateTime::Now must create a DateTime with the current timestamp", []() -> string
  {
    time_t timestamp = std::time(0);

    if (timestamp != (int)DateTime::Now())
      return ("timestamps didn't match");
    return ("");
  });
}
