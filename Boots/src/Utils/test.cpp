#include <Utils/test.hpp>
#include <Utils/shell.hpp>
#include <algorithm>
#include <sstream>
#include <iostream>

using namespace std;

void       UnitTest::AddTest(const std::string& category, const std::string& description, std::function<std::string (void)> callback)
{
  auto     cat = std::find(_categories.begin(), _categories.end(), category);
  Test     new_test;

  if (description.size() > _longest_desc)
    _longest_desc = description.size();
  new_test.description = description;
  new_test.callback    = callback;
  if (cat == _categories.end())
  {
    _categories.push_back(Category(category));
    cat = (--(_categories.end()));
  }
  cat->tests.push_back(new_test);
}

void       UnitTest::Execute(void)
{
  stringstream   recap_stream;
  ShellString    recap, recap_head;
  unsigned short n_tests   = 0;
  unsigned short n_success = 0;

  std::for_each(_categories.begin(), _categories.end(), [this, &n_tests, &n_success](Category category)
  {
    ShellString str_category(category.name);

    str_category.Bolded();
    cout << str_category << endl;
    std::for_each(category.tests.begin(), category.tests.end(), [this, &n_tests, &n_success](Test test)
    {
      ShellString str_description(test.description);
      ShellString str_success;
      ShellString str_message;

      n_tests++;
      str_description.Colorize("magenta");
      cout << "-> [" << str_description << "] ";
      for (unsigned short i = test.description.size() ; i < _longest_desc ; ++i)
        cout << ' ';
      str_message = test.callback();
      if (str_message.size() == 0) // Success
      {
        n_success++;
        str_success = "Success";
        str_success.Colorize("green");
      }
      else // Failure
      {
        str_success = "Failure:";
        str_success.Colorize("red");
        str_message.Colorize("gray");
        str_message.Backgroundize("red");
      }
      cout << str_success << " " << str_message << endl;
    });
  });
  recap_stream << n_success << '/' << n_tests;
  recap = recap_stream.str();
  recap.Blinking();
  recap_head = "Test Results";
  recap_head.Underlined();
  cout << recap_head << ": " << recap << " (" << (((float)n_success / n_tests) * 100) << "%)" << endl;
}

