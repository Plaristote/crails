#ifndef  CRAILS_TESTS_SELENIUM_HELPER_HPP
# define CRAILS_TESTS_SELENIUM_HELPER_HPP

# include <crails/tests/helper.hpp>
# include <crails/tests/selenium.hpp>

namespace Crails
{
  namespace Tests
  {
    struct SeleniumHelper : public Crails::Tests::Helper
    {
      std::unique_ptr<SeleniumDriver> page;
      std::string browser = "firefox";

      SeleniumHelper(const std::string& name) : Helper(name)
      {
        before([this]() {
          page.reset(new SeleniumDriver);
          page->use_browser(browser);
        });

        after([this]() {
          page.reset(nullptr);
        });
      }
    };
  }
}

#endif
