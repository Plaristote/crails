#include "crails/tests/helper.hpp"
#include <crails/logger.hpp>
#include <algorithm>
#include <iostream>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"

using namespace std;
using namespace Crails;
using namespace Crails::Tests;

Helper::Helper(const string& name) : name(name)
{
  passed_count = failed_count = pending_count = 0;
}

void Helper::run()
{
  logger.set_stdout(current_test_stdout);
  logger.set_stderr(current_test_stdout);
  cout << name << ':' << endl;
  for_each(groups.begin(), groups.end(), [this](Group group)
  {
    cout << "  " << group.name << ':' << endl;
    for_each(group.tests.begin(), group.tests.end(), [this, group](Test test)
    {
      current_test_stdout.str(string());
      current_test_output.str(string());
      current_test_stdout.clear();
      current_test_output.clear();
      if (test.pending == false)
        run_protected_test(group, test);
      display_test_results(test);
    });
  });
}

void Helper::run_test(Group group, Test test)
{
  current_test_result = true;
  if (before_all)
    before_all();
  if (group.before)
    group.before();
  test.callback();
  if (group.after)
    group.after();
  if (after_all)
    after_all();
}

void Helper::run_protected_test(Group group, Test test)
{
  try {
    run_test(group, test);
  }
  catch (const ExpectationFailed& e)
  {
    current_test_result = false;
    current_test_output << "      Expectation failed: `" << e.expectation_text << "` at " << e.file_reference << endl;
    current_test_output << "      - expected: " << e.expected << endl;
    current_test_output << "      - obtained: " << e.obtained << endl;
  }
  catch (const std::exception& e)
  {
    stringstream trace;
    trace << boost_ext::trace(e);
    current_test_result = false;
    current_test_output << "      Exception catched: '" << e.what() << "'" << endl;
    if (trace.str().size() > 0)
      current_test_output << "      Backtrace: " << endl << trace.str() << endl;
  }
  catch (...) {
    current_test_result = false;
    current_test_output << "      Exception catched: Unknown exception type." << endl;
  }
}

void Helper::display_test_results(Test test)
{
  cout << "    " << test.description << ':';
  if (test.pending)
  {
    cout << KYEL << " Pending" << KNRM << endl;
    pending_count++;
  }
  else if (current_test_result)
  {
    cout << KGRN << " Passed" << KNRM << endl;
    passed_count++;
  }
  else
  {
    cout << KRED << " Failed" << endl;
    cerr << current_test_output.str() << KNRM << endl;
    cerr << current_test_stdout.str() << endl;
    failed_count++;
  }
}