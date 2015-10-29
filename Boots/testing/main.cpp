#include <Boots/Utils/test.hpp>
#include <algorithm>

using namespace std;

void TestsData(UnitTest&);
void TestsJSON(UnitTest&);
void TestsSerializer(UnitTest&);
void TestsDirectory(UnitTest&);
void TestFlyweight(UnitTest&);
void TestFunctorThread(UnitTest&);
void TestSingleton(UnitTest&);
void TestSemaphore(UnitTest&);

typedef void (*FuncInitializer)(UnitTest&);

int main(int ac, char **av)
{
  std::list<FuncInitializer> TestInitializers;
  UnitTest tester;

  TestInitializers.push_back(TestsDirectory);
  TestInitializers.push_back(TestsData);
  TestInitializers.push_back(TestsJSON);
  //TestInitializers.push_back(TestsSerializer);
  TestInitializers.push_back(TestFlyweight);
  TestInitializers.push_back(TestSemaphore);
  TestInitializers.push_back(TestSingleton);

  for_each(TestInitializers.begin(), TestInitializers.end(), [&tester](FuncInitializer f) { f(tester); });

  tester.Execute();

  return (0);
}
