#ifndef  CRAILS_TESTS_SELENIUM_HPP
# define CRAILS_TESTS_SELENIUM_HPP

# include <webdriverxx/webdriverxx.h>

namespace Crails
{
  namespace Tests
  {
    class SeleniumDriver;

    class SeleniumExpectation
    {
      friend class SeleniumDriver;
      SeleniumDriver&      driver;
      webdriverxx::Element element;
      std::string          selector;

      SeleniumExpectation(SeleniumDriver& driver) : driver(driver)
      {
      }

    public:
      SeleniumExpectation& to_have_element(const std::string& selector);

      SeleniumExpectation& with_content(const std::string& content);
      SeleniumExpectation& selected();
      SeleniumExpectation& not_selected();
      SeleniumExpectation& visible();
      SeleniumExpectation& not_visible();
    };

    class SeleniumDriver
    {
      static const unsigned short default_max_wait_time = 5;
    public:
      SeleniumDriver() : max_wait_time(default_max_wait_time)
      {}

      ~SeleniumDriver()
      {}

      SeleniumDriver& visit(const std::string& url)
      {
        get_driver().Navigate(url);
        return *this;
      }

      SeleniumDriver& back()
      {
	get_driver().Back();
        return *this;
      }

      SeleniumDriver& forward()
      {
        get_driver().Forward();
        return *this;
      }

      SeleniumDriver& refresh()
      {
        get_driver().Refresh();
        return *this;
      }

      webdriverxx::Element find(const std::string& selector)
      {
        return get_driver().FindElement(webdriverxx::ByCss(selector));
      }

      SeleniumExpectation expect()
      {
        return SeleniumExpectation(*this);
      }

      void close()
      {
        if (driver_initialized)
        {
          driver.reset(nullptr);
          driver_initialized = false;
	}
      }

      void use_browser(const std::string& name)
      {
        close();
        browser_name = name;
      }

      void set_max_wait_time(unsigned short wait_time)
      {
        max_wait_time = wait_time;
        if (driver_initialized)
          driver->SetTimeoutMs(webdriverxx::timeout::Implicit, wait_time * 1000);
      }

      inline webdriverxx::WebDriver& get_driver()
      {
        if (!driver_initialized)
          initialize_driver();
        return *driver;
      }

    private:
      void initialize_driver()
      {
        auto new_driver = webdriverxx::Start(webdriverxx::Capabilities().SetBrowserName(browser_name));

        driver.reset(new webdriverxx::WebDriver(new_driver));
        driver_initialized = true;
        set_max_wait_time(max_wait_time);
      }

      bool                                    driver_initialized = false;
      unsigned short                          max_wait_time;
      std::unique_ptr<webdriverxx::WebDriver> driver;
      std::string                             browser_name = "firefox";
    };
  }
}

#endif
