#include <Boots/Utils/test.hpp>
#include <algorithm>

using namespace std;

void TestsDirectory(UnitTest&);
void TestFlyweight(UnitTest&);
void TestFunctorThread(UnitTest&);
void TestSingleton(UnitTest&);

typedef void (*FuncInitializer)(UnitTest&);

int main(int ac, char **av)
{
  std::list<FuncInitializer> TestInitializers;
  UnitTest tester;

  TestInitializers.push_back(TestsDirectory);
  TestInitializers.push_back(TestFlyweight);
  TestInitializers.push_back(TestSingleton);

  for_each(TestInitializers.begin(), TestInitializers.end(), [&tester](FuncInitializer f) { f(tester); });

  tester.Execute();

  return (0);
}
