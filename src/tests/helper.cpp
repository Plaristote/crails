#include "crails/tests/helper.hpp"
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
  cout << name << ':' << endl;
  for_each(groups.begin(), groups.end(), [this](Group group)
  {
    cout << "  " << group.first << ':' << endl;
    for_each(group.second.begin(), group.second.end(), [this](Test test)
    {
      current_test_output = stringstream();
      cout << "    " << test.description << ':';
       try {
         current_test_result = true;
         if (test.pending == false)
           test.callback();
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
         failed_count++;
       }
     });
   });
 }
