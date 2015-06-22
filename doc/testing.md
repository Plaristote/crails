# Crails Test Framework
By default, whenever you build the project, a `tests` executable is built along with it. In this section,
we'll explain how to add tests than are going to be run by `tests`.

## Adding a test suite
A test suite in Crails is a class inheriting `Crails::Tests::Helper`. It allows you to structure your tests,

```C++
#ifndef  MY_TEST_SUITE_HPP
# define MY_TEST_SUITE_HPP

# include <crails/tests/helper.hpp>

class MyTestSuite : public Crails::Tests::Helper
{
public:
  MyTestSuite() : Crails::Tests::Helper("MyTest")
  {
    describe("some_method", [this]()
    {
      it("should check something", [this]()
      {
        EXPECT("value", ==, "value")
      });
    });
  }
}
#endif
```

Then, open the `spec/spec.cpp` file. When the project is generated, this file is created with two `setup` and `shutdown` methods, which will be executed before and after all the tests are run. You must register your test classes in the `setup` method, like this:

```C++
void Crails::Tests::Runner::setup()
{
  helpers.push_back(new MyTestSuite);
}
```

Now, when you run the `tests` executable in your build directory, `MyTestSuite` will be run.

### Running code before/after the tests
You may want to run code to prepare and cleanup your tests: and that's why the methods `before` and `after` are here for you.
Here's a sample of code showing you how they'd run:

```C++
class MyTestSuite : public Crails::Tests::Helper
{
  // You may define instances variables here so that they may
  // be shared between all your tests.
public:
  MyTestSuite() : Crails::Tests::Helper("MyTest")
  {
    before([this]()
    {
      // This lambda will be run before all the tests from this suite
    });

    after([this]()
    {
      // This lambda will be run after all the tests from this suite
    });

    describe("some_method", [this]()
    {
      before([this]()
      {
        // This lambda will be run before the tests from this `describe`
      });

      it("should check something", [this]()
      {
        EXPECT("value", ==, "value")
      });
    });
  }
}
```

### Pending test
It's sometimes useful to have the ability to disable some tests from your suite. When you want set a test as 'pending', just replace `it` with `xit`, such as this:

```C++
  describe("my_method", [this]()
  {
    xit("this test won't run", [this]() {});
    it("this test will run", [this]() {});
  });
```

## Request tests
Request tests are a good way of quickly telling whether your server works or not. Use the `Crails::Tests::Request` to simulate requests made to your server, such as this:

```C++
  describe("index", [this]()
  {
    it("should return the index", [this]()
    {
      Crails::Tests::Request request("GET", "/users");

      request.run();
      EXPECT(request.response["body"].Value(), ==, "{users:[{email:'test@domaincom'}]}")
    });
  });
```

The request object also contains a `Params` object that you can use to further customise your request:

```C++
  describe("create", [this]()
  {
    it("should create an user", [this]()
    {
      unsigned short user_count = Criteria<User>::Query()->Count();
      Crails::Tests::Request request("POST", "/users");
      
      request.params["user"]["email"] = "test@domain.com";
      request.params["user"]["password"] = "password";
      request.run();
      EXPECT(Criteria<User>::Query()->Count(), ==, (user_count + 1))
    });
  });
```
