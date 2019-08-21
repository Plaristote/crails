#include "crails/tests/selenium.hpp"
#include "crails/tests/helper.hpp"

using namespace Crails::Tests;
using namespace webdriverxx;

SeleniumExpectation& SeleniumExpectation::to_have_element(const std::string& selector)
{
  this->selector = selector;
  try
  {
    element = driver.get_driver().FindElement(webdriverxx::ByCss(selector));
  }
  catch (...)
  {
    throw ExpectationFailed("no elements matching `" + selector + "` found", "", "true", "false");
  }
  return *this;
}

SeleniumExpectation& SeleniumExpectation::with_content(const std::string& content)
{
  if (element.GetText() != content)
    throw ExpectationFailed("element " + selector + " does not have expected content", "", content, element.GetText());
  return *this;
}

SeleniumExpectation& SeleniumExpectation::selected()
{
  if (!element.IsSelected())
    throw ExpectationFailed("element " + selector + " should be selected", "", "true", "false");
  return *this;
}

SeleniumExpectation& SeleniumExpectation::not_selected()
{
  if (element.IsSelected())
    throw ExpectationFailed("element " + selector + " should not be selected", "", "false", "true");
  return *this;
}

SeleniumExpectation& SeleniumExpectation::visible()
{
  if (!element.IsDisplayed())
    throw ExpectationFailed("element" + selector + " should be displayed", "", "true", "false");
  return *this;
}

SeleniumExpectation& SeleniumExpectation::not_visible()
{
  if (!element.IsDisplayed())
    throw ExpectationFailed("element" + selector + " should not be displayed", "", "false", "true");
  return *this;
}
