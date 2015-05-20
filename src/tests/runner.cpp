#include "crails/tests/runner.hpp"
#include <iostream>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"

using namespace std;
using namespace Crails;
using namespace Crails::Tests;

bool Runner::execute()
{
  unsigned short passed_count, failed_count, pending_count;

  passed_count = failed_count = pending_count = 0;
  cout << "Crails::Tests::Runner: Executing all tests" << endl;
  for_each(helpers.begin(), helpers.end(), [&passed_count, &failed_count, &pending_count](SP(Helper) helper)
  {
    helper->run();
    passed_count  += helper->passed_count;
    failed_count  += helper->failed_count;
    pending_count += helper->pending_count;
  });
  cout << endl;
  cout << "Report: " << KGRN << passed_count << " passed, " << KRED << failed_count << " failed, " << KYEL << pending_count << " pendings" << KNRM << endl;
  return failed_count == 0;
}
