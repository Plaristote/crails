#ifndef  CRAILS_TESTS_HELPER_HPP
# define CRAILS_TESTS_HELPER_HPP

# include "params.hpp"
# include <sstream>
# include <functional>

# define EXPECT(expression1,op,expression2) \
  if (!(expression1 op expression2)) \
  { \
    std::stringstream expectation_text, file_reference, expression1_str, expression2_str; \
    expectation_text << #expression1 << ' ' << #op << ' ' << #expression2; \
    file_reference   << __FILE__ << ':' << __LINE__; \
    expression1_str << expression1; \
    expression2_str << expression2; \
    throw Crails::Tests::ExpectationFailed(expectation_text.str(), file_reference.str(), expression1_str.str(), expression2_str.str()); \
  }

namespace Crails
{
  namespace Tests
  {
    struct ExpectationFailed
    {
      ExpectationFailed(const std::string& expectation_text, const std::string& file_reference, const std::string& obtained, const std::string& expected)
        : expectation_text(expectation_text), file_reference(file_reference), expected(expected), obtained(obtained)
      {
      }

      std::string expectation_text, file_reference, expected, obtained;
    };

    struct Helper
    {
      Helper(const std::string& name);
      void run();

    protected:
      struct Test
      {
        bool                   pending;
        std::string            description;
        std::function<void ()> callback;
      };
      
      struct Group
      {
        std::string            name;
        std::list<Test>        tests;
        std::function<void ()> before, after;
      };

      typedef std::list<Group> Groups;
      
    private:
      void run_test(Group, Test);
      void run_protected_test(Group, Test);
      void after_test(Group);
      void display_test_results(Test);
      
    protected:
      void before(std::function<void()> callback)
      {
        if (current_group.name == "")
          before_all = callback;
        else
          current_group.before = callback;
      }
      
      void after(std::function<void()> callback)
      {
        if (current_group.name == "")
          after_all = callback;
        else
          current_group.after = callback;
      }
      
      void describe(const std::string& name, std::function<void ()> block)
      {
        current_group.name = name;
        block();
        groups.push_back(current_group);
        current_group.tests.clear();
        current_group.name = "";
      }

      void xit(const std::string& description, std::function<void ()> block)
      {
        Test test;

        test.pending     = true;
        test.description = description;
        current_group.tests.push_back(test);
      }

      void it(const std::string& description, std::function<void ()> block)
      {
        Test test;

        test.pending     = false;
        test.description = description;
        test.callback    = block;
        current_group.tests.push_back(test);
      }

    private:
      Groups            groups;
      Group             current_group;
      std::function<void()> before_all, after_all;
    public:
      bool              current_test_result;
      std::stringstream current_test_output;
      std::stringstream current_test_stdout;
      std::string       name;

      unsigned short passed_count, failed_count, pending_count;
    };
  }
}

#endif
