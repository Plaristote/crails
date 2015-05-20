#ifndef  CRAILS_TESTS_HELPER_HPP
# define CRAILS_TESTS_HELPER_HPP

# include "../params.hpp"
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
      ExpectationFailed(const std::string& expectation_text, const std::string& file_reference, const std::string& expected, const std::string& obtained)
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

      typedef std::pair<std::string, std::list<Test> >        Group;
      typedef std::list<Group>                                Groups;

      void describe(const std::string& name, std::function<void ()> block)
      {
        current_group.first = name;
        block();
        groups.push_back(current_group);
      }

      void xit(const std::string& description, std::function<void ()> block)
      {
        Test test;

        test.pending     = true;
        test.description = description;
        current_group.second.push_back(test);
      }

      void it(const std::string& description, std::function<void ()> block)
      {
        Test test;

        test.pending     = false;
        test.description = description;
        test.callback    = block;
        current_group.second.push_back(test);
      }

    private:
      Groups            groups;
      Group             current_group;
    public:
      bool              current_test_result;
      std::stringstream current_test_output;
      std::string       name;

      unsigned short passed_count, failed_count, pending_count;
    };
  }
}

#endif
